#include "ChargeControllerServer.h"

ChargeControllerServer::ChargeControllerServer(IServerAdapter& serverAdapter)
    : m_serverAdapter(serverAdapter)
{
    m_serverAdapter.onClientConnected([this](const uint16_t endpointId) { onClientConnected(endpointId); });
    m_serverAdapter.onClientDisconnected([this](const uint16_t endpointId) { onClientDisconnected(endpointId); });
    m_serverAdapter.onClientDataReceived([this](const uint16_t endpointId) { onClientDataReceived(endpointId); });
}

void ChargeControllerServer::onClientConnected(const uint16_t endpointId)
{
    m_packetParsers[endpointId].reset();
}

void ChargeControllerServer::onClientDisconnected(const uint16_t endpointId)
{
    m_packetParsers.erase(endpointId);
}

void ChargeControllerServer::onClientDataReceived(const uint16_t endpointId)
{
    char buf[128] = { 0 };
    auto&& packetParser = m_packetParsers[endpointId];

    while (m_serverAdapter.availableDataBytes(endpointId) > 0)
    {
        auto read = m_serverAdapter.readClientData(endpointId, buf, sizeof(buf));
        packetParser.input(buf, read);
    };

    if (packetParser.isCompleted())
    {
        handleIncomingPacket(endpointId, packetParser.payload());
        packetParser.reset();
    }
}

void ChargeControllerServer::handleIncomingPacket(const uint16_t endpointId, const char* payload)
{
    const char* begin = payload;
    const char* end = payload + strlen(payload);

    auto json = nlohmann::json::parse(begin, end, nullptr, false);

    if (!isBaseRequestPacketValid(json))
    {
        sendErrorResponseToClient(endpointId, json, ErrorCode::MalformedRequest);
        return;
    }

    if (!routeRequest(endpointId, json))
    {
        sendErrorResponseToClient(endpointId, json, ErrorCode::UnknownRequestType);
    }
}

bool ChargeControllerServer::isBaseRequestPacketValid(const nlohmann::json& req) const
{
    return req.is_object()
        && hasField(req, "type") && req["type"].is_string()
        && hasField(req, "id") && req["id"].is_number_integer()
        && (!hasField(req, "param") || req["param"].is_object())
        ;
}

bool ChargeControllerServer::routeRequest(const uint16_t endpointId, const nlohmann::json& req)
{
    auto&& t = req["type"];

    if (t == "set-switch")
    {
        switchController(endpointId, req);
        return true;
    }
    else if (t == "get-status")
    {
        sendStatusResponse(endpointId, req);
        return true;
    }

    return false;
}

void ChargeControllerServer::switchController(const uint16_t endpointId, const nlohmann::json& req)
{
    if (!hasParam(req, "on"))
    {
        sendErrorResponseToClient(endpointId, req, ErrorCode::MissingMandatoryParameter);
        return;
    }

    m_switchModule.setState(req["param"]["on"] != 0);

    sendSuccessResponseToClient(endpointId, req["id"]);
}

void ChargeControllerServer::sendStatusResponse(const uint16_t endpointId, const nlohmann::json& req)
{
    nlohmann::json json;

    json["switch"] = m_switchModule.state();

    sendSuccessResponseToClient(endpointId, req["id"], 0, json);
}

void ChargeControllerServer::sendErrorResponseToClient(const uint16_t endpointId, const nlohmann::json& req, const ErrorCode errorCode)
{
    nlohmann::json json;

    if (hasField(req, "id") && req["id"].is_number_integer())
        json["id"] = req["id"];

    json["error"] = errorCode;

    sendJsonToClient(endpointId, json);
}

void ChargeControllerServer::sendSuccessResponseToClient(const uint16_t endpointId, int reqId, int resultCode, const nlohmann::json& param)
{
    nlohmann::json resp;

    resp["id"] = reqId;
    resp["result"] = resultCode;

    if (!param.is_null())
        resp["param"] = param;

    sendJsonToClient(endpointId, resp);
}

void ChargeControllerServer::sendJsonToClient(const uint16_t endpointId, const nlohmann::json& resp)
{
    auto&& s = resp.dump();

    m_serverAdapter.sendDataToClient(endpointId, s.c_str(), s.size());
    m_serverAdapter.sendDataToClient(endpointId, "\r\n", 2);
}

bool ChargeControllerServer::hasField(const nlohmann::json& json, const std::string& name)
{
    return json.find(name) != json.cend();
}

bool ChargeControllerServer::hasParam(const nlohmann::json& json, const std::string& name)
{
    return hasField(json, "param") && json["param"].is_object() && hasField(json["param"], name);
}


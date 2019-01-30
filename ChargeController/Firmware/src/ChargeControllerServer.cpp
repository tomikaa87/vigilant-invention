#include "ChargeControllerServer.h"

#include <Arduino.h>

ChargeControllerServer::ChargeControllerServer(IServerAdapter& serverAdapter, LedController& ledController)
    : m_serverAdapter(serverAdapter)
    , m_ledController(ledController)
{
    m_serverAdapter.onClientConnected([this](const uint16_t endpointId) { onClientConnected(endpointId); });
    m_serverAdapter.onClientDisconnected([this](const uint16_t endpointId) { onClientDisconnected(endpointId); });
    m_serverAdapter.onClientDataReceived([this](const uint16_t endpointId) { onClientDataReceived(endpointId); });

    m_keepAliveTimer.setTimeoutCallback([this] { onKeepAliveTimerTimeout(); });
}

void ChargeControllerServer::task()
{
    m_keepAliveTimer.task();
}

void ChargeControllerServer::onClientConnected(const uint16_t endpointId)
{
    Serial.printf("CCS: packet parser created for endpoint: %d\r\n", endpointId);

    m_packetParsers[endpointId].reset();
}

void ChargeControllerServer::onClientDisconnected(const uint16_t endpointId)
{
    m_packetParsers.erase(endpointId);

    Serial.printf("CCS: removed packet parser of endpoint: %d. Remaining: %d\r\n",
        endpointId, m_packetParsers.size());
}

void ChargeControllerServer::onClientDataReceived(const uint16_t endpointId)
{
    Serial.printf("CCS: client data received, endpoindId: %d\r\n", endpointId);

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

void ChargeControllerServer::onKeepAliveTimerTimeout()
{
    Serial.println("CCS: keep-alive timer timeout");

    if (m_switchModule.state())
    {
        m_ledController.setLed(LedController::Led::Red, LedController::State::On, 2000);
        m_switchModule.setState(false);
    }
}

void ChargeControllerServer::handleIncomingPacket(const uint16_t endpointId, const char* payload)
{
    const auto begin = payload;
    const auto end = payload + strlen(payload);

    auto json = nlohmann::json::parse(begin, end, nullptr, false);

    if (!isBaseRequestPacketValid(json))
    {
        sendErrorResponseToClient(endpointId, json, ErrorCode::MalformedRequest);
        return;
    }

    if (!routeRequest(endpointId, json))
    {
        sendErrorResponseToClient(endpointId, json, ErrorCode::UnknownRequestType);
        m_ledController.setLed(LedController::Led::Red, LedController::State::On, 1000);
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
    std::string t = req["type"];

    Serial.printf("CCS: routing request: %s\n", t.c_str());

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
    else if (t == "keep-alive")
    {
        keepAlive(endpointId, req);
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

    const bool on = req["param"]["on"] != 0;

    m_switchModule.setState(on);
    m_ledController.setLed(on ? LedController::Led::Green : LedController::Led::Red, LedController::State::On, 2000);

    sendSuccessResponseToClient(endpointId, req["id"]);
}

void ChargeControllerServer::sendStatusResponse(const uint16_t endpointId, const nlohmann::json& req)
{
    nlohmann::json json;

    json["switch"] = m_switchModule.state();

    sendSuccessResponseToClient(endpointId, req["id"], 0, json);
}

void ChargeControllerServer::keepAlive(const uint16_t endpointId, const nlohmann::json& req)
{
    if (!hasParam(req, "timeout"))
    {
        sendErrorResponseToClient(endpointId, req, ErrorCode::MissingMandatoryParameter);
        return;
    }

    int timeout = req["param"]["timeout"];

    Serial.printf("CCS: keep-alive received, timeout: %d\n", timeout);

    m_keepAliveTimer.restart(timeout);

    sendSuccessResponseToClient(endpointId, req["id"]);
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


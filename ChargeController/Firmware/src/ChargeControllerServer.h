#pragma once

#include "ClientPacketParser.h"
#include "IServerAdapter.h"
#include "SwitchModule.h"
#include "Timer.h"

#include "json.hpp"

#include <functional>
#include <memory>
#include <map>

class ChargeControllerServer
{
public:
    ChargeControllerServer(IServerAdapter& serverAdapter);

    void task();

private:
    IServerAdapter& m_serverAdapter;

    std::map<uint16_t, ClientPacketParser> m_packetParsers;

    SwitchModule m_switchModule;
    Timer m_keepAliveTimer;

    void onClientConnected(const uint16_t endpointId);
    void onClientDisconnected(const uint16_t endpointId);
    void onClientDataReceived(const uint16_t endpointId);

    void onKeepAliveTimerTimeout();

    void handleIncomingPacket(const uint16_t endpointId, const char* payload);
    bool isBaseRequestPacketValid(const nlohmann::json& json) const;
    bool routeRequest(const uint16_t endpointId, const nlohmann::json& req);

    void switchController(const uint16_t endpointId, const nlohmann::json& req);
    void sendStatusResponse(const uint16_t endpointId, const nlohmann::json& req);
    void keepAlive(const uint16_t endpointId, const nlohmann::json& req);

    enum class ErrorCode : int
    {
        InternalError,
        MalformedRequest,
        UnknownRequestType,
        MissingMandatoryParameter
    };

    void sendErrorResponseToClient(const uint16_t endpointId, const nlohmann::json& req, const ErrorCode errorCode);
    void sendSuccessResponseToClient(const uint16_t endpointId, int reqId, int resultCode = 0, const nlohmann::json& param = {});

    void sendJsonToClient(const uint16_t endpointId, const nlohmann::json& resp);

    static bool hasField(const nlohmann::json& json, const std::string& name);
    static bool hasParam(const nlohmann::json& json, const std::string& name);
};


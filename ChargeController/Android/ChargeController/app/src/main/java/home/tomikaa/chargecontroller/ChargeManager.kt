package home.tomikaa.chargecontroller

import android.util.Log
import androidx.annotation.WorkerThread
import androidx.lifecycle.MutableLiveData
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import org.json.JSONObject
import java.io.IOException
import java.net.InetSocketAddress
import java.net.Socket
import java.net.SocketTimeoutException
import java.util.*
import kotlin.concurrent.scheduleAtFixedRate

class ChargeManager {

    val outputEnabled by lazy {
        MutableLiveData<Boolean>()
    }

    var deviceResponsive = true
        private set

    var chargeLowerLimit = Constants.defaultChargeLowerLimit
    var chargeUpperLimit = Constants.defaultChargeUpperLimit
    var changedCallback: (() -> Unit)? = null

    val currentChargeLevel by lazy {
        MutableLiveData<Int>()
    }

    val pluggedIn by lazy {
        MutableLiveData<Boolean>()
    }

    private val defaultScope = CoroutineScope(Dispatchers.Default)
    private val uiScope = CoroutineScope(Dispatchers.Main)

    private val keepAliveTimer = Timer("keepAliveTimer")
    private var keepAliveTimerTask: TimerTask? = null
    private val keepAliveSendInterval: Long = 60

    init {
        updateStatus()
    }

    fun handleBatteryChanged(level: Int, pluggedIn: Boolean) {
        Log.i(tag, "Battery state changed. Charge level: $level. Plugged in? ${if (pluggedIn) "Yes" else "No"}")

        currentChargeLevel.value = level
        this.pluggedIn.value = pluggedIn

        if (level >= 80 && outputEnabled.value == true) {
            disableOutput()
        } else if (level <= 70 && outputEnabled.value == false) {
            enableOutput()
        }
    }

    fun enableOutput() {
        setOutput(true)
    }

    fun disableOutput() {
        setOutput(false)
    }

    fun setOutput(enabled: Boolean) {
        defaultScope.launch {
            val id = nextPacketId++

            val response = sendDeviceRequest(createSwitchPacket(enabled, id))

            if (response != null) {
                Log.d(tag, "setOutput: response: $response")

                if (response.optInt("id", -1) != id) {
                    Log.w(tag, "setOutput: packet ID mismatch")
                    return@launch
                }

                val result = response.getInt("result")

                if (result != 0) {
                    Log.w(tag, "setOutput: switch operation failed, result: $result")
                }
            }

            updateStatus()
        }
    }

    fun toggleOutput() {
        if (isEnabled())
            disableOutput()
        else
            enableOutput()
    }

    fun isEnabled(): Boolean {
        return outputEnabled.value == true
    }

    private fun createSwitchPacket(on: Boolean, id: Int): JSONObject {
        var json = createBasicCommandPacket("set-switch", id)
        val param = JSONObject()
        param.put("on", if (on) 1 else 0)
        json.put("param", param)
        return json
    }

    private fun createStatusRequestPacket(id: Int): JSONObject {
        return createBasicCommandPacket("get-status", id)
    }

    private fun updateStatus() {
        Log.d(tag, "Updating device status")

        defaultScope.launch {
            val id = nextPacketId++

            val response = sendDeviceRequest(createStatusRequestPacket(id))

            if (response != null) {
                Log.d(tag, "updateStatus: response: $response")

                if (response.optInt("id", -1) != id) {
                    Log.w(tag, "updateStatus: packet ID mismatch")
                    return@launch
                }

                if (!response.has("param")) {
                    Log.w(tag, "updateStatus: param is missing from the response")
                    return@launch
                }

                val param = response.getJSONObject("param")

                if (!param.has("switch")) {
                    Log.w(tag, "updateStatus: switch parameter is missing from the response")
                    return@launch
                }

                val switchState = response.getJSONObject("param").getBoolean("switch")

                if (switchState)
                    startKeepAliveTimer()
                else
                    stopKeepAliveTimer()

                uiScope.launch {
                    outputEnabled.value = switchState
                }
            }

            uiScope.launch {
                changedCallback?.invoke()
            }
        }
    }

    private fun sendKeepAlive() {
        val id = nextPacketId++

        val json = createBasicCommandPacket("keep-alive", id)
        val param = JSONObject()
        param.put("timeout", keepAliveSendInterval + 30)
        json.put("param", param)

        Log.d(tag,"Sending keep-alive packet")

        defaultScope.launch {
            val response = sendDeviceRequest(json)

            if (response != null) {
                Log.d(tag, "sendKeepAlive response: $response")

                if (response.optInt("id", -1) != id) {
                    Log.w(tag, "sendKeepAlive: packet ID mismatch")
                    return@launch
                }

                val resultCode = response.optInt("result", -1)
                if (resultCode != 0) {
                    Log.w(tag, "sendKeepAlive: command failed, result code: $resultCode")
                    return@launch
                }
            } else {
                Log.w(tag, "sendKeepAlive: no response from the device")
            }
        }
    }

    private fun startKeepAliveTimer() {
        if (keepAliveTimerTask != null) {
            Log.w(tag, "Keep-alive timer is already running")
            return
        }

        Log.d(tag, "Starting keep-alive timer")

        keepAliveTimerTask = keepAliveTimer.scheduleAtFixedRate(0, keepAliveSendInterval * 1000) {
            sendKeepAlive()
        }
    }

    private fun stopKeepAliveTimer() {
        Log.d(tag, "Stopping keep-alive timer")

        keepAliveTimerTask?.cancel()
        keepAliveTimerTask = null
    }

    private fun createBasicCommandPacket(type: String, id: Int): JSONObject {
        var json = JSONObject()
        json.put("type", type)
        json.put("id", id)
        return json
    }

    private suspend fun sendDeviceRequest(req: JSONObject): JSONObject? {
        val request = createRequestDatagram(req)
        var response: ByteArray?

        for (i in 1..3) {
            response = sendDatagram(request, InetSocketAddress("192.168.0.7", 52461))

            if (response != null)
                return JSONObject(response.toString(Charsets.UTF_8))
        }

        return null
    }

    private fun createRequestDatagram(req: JSONObject): ByteArray {
        val payload = req.toString(0).toByteArray()
        val payloadLength: Int = payload.size

        val header = ByteArray(6)
        header[0] = 'K'.toByte()
        header[1] = 'T'.toByte()
        header[2] = (payloadLength and 0xff).toByte()
        header[3] = (payloadLength shr 8).toByte()
        header[4] = 'T'.toByte()
        header[5] = 'K'.toByte()

        return header + payload
    }

    @WorkerThread
    private suspend fun sendDatagram(datagram: ByteArray, address: InetSocketAddress): ByteArray? {
        val socket = Socket()

        Log.d(tag, "Sending datagram to the device")

        socket.use {
            try {
                socket.connect(address, 3000)
            } catch (e: SocketTimeoutException) {
                Log.w(tag, "Connection timed out")
                deviceResponsive = false
                return null
            } catch (e: Exception) {
                Log.w(tag, "Exception: $e")
                return null
            }

            try {
                val out = socket.getOutputStream()
                out.write(datagram)
            } catch (e: IOException) {
                Log.w(tag, "Failed to send datagram. Error: $e")
                return null
            }

            val inputStream = socket.getInputStream()

            for (i in 1..3) {
                if (inputStream.available() == 0)
                    delay(500)
            }

            if (inputStream.available() == 0) {
                Log.w(tag, "No response from the device")
                deviceResponsive = false
                return null
            }

            deviceResponsive = true

            val buffer = ByteArray(inputStream.available())
            inputStream.read(buffer)

            Log.d(tag, "Response received from the device")

            return buffer
        }
    }

    companion object {
        const val tag = "ChargeManager"
        var nextPacketId = 0
    }
}
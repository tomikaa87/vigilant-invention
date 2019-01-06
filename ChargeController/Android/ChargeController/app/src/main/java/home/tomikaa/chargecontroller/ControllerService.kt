package home.tomikaa.chargecontroller

import android.app.Notification
import android.app.PendingIntent
import android.app.Service
import android.content.Intent
import android.content.IntentFilter
import android.os.IBinder
import android.support.v4.app.NotificationCompat
import android.support.v4.content.LocalBroadcastManager
import android.util.Log

class ControllerService : Service() {

    private var notification: Notification? = null
    private val batteryChangeBroadcastReceiver = BatteryChangeBroadcastReceiver()
    private val chargeManager = ChargeManager()
    private var serviceActive = false
    private var preferencesChangeListenerRegistered = false


    override fun onBind(intent: Intent): IBinder {
        TODO("Return the communication channel to the service.")
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        when (intent?.action) {
            Constants.startForegroundAction -> {
                if (!serviceActive) {
                    Log.i(tag, "StartAction")
                    if (notification == null)
                        notification = buildNotification()

                    if (!preferencesChangeListenerRegistered)
                        registerPreferenceChangeListener()

                    registerBroadcastReceiver()
                    startForeground(Constants.notificationId, notification)
                    serviceActive = true
                    sendStatusUpdate()
                }
            }

            Constants.stopForegroundAction -> {
                if (serviceActive) {
                    Log.i(tag, "StopAction")
                    stopForeground(true)
                    serviceActive = false
                    sendStatusUpdate()
                    unregisterReceiver(batteryChangeBroadcastReceiver)
                    stopSelf()
                }
            }

            Constants.enablePowerOutput -> {
                Log.i(tag, "EnablePowerOutputAction")
                chargeManager.enableOutput()
            }

            Constants.disablePowerOutput -> {
                Log.i(tag, "DisablePowerOutputAction")
                chargeManager.disableOutput()
            }

            Constants.togglePowerOutput -> {
                Log.i(tag, "TogglePowerOutputAction")
                chargeManager.toggleOutput()
            }

            Constants.sendStatusUpdate -> {
                Log.i(tag, "SendStatusUpdateAction")
                sendStatusUpdate()
            }
        }

        return START_STICKY
    }

    override fun onCreate() {
        super.onCreate()

        batteryChangeBroadcastReceiver.callback = chargeManager::handleBatteryChanged
        chargeManager.changedCallback = this::sendStatusUpdate
    }

    private fun buildNotification(): Notification {
        val notificationIntent = Intent(this, MainActivity::class.java)
        notificationIntent.flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
        notificationIntent.action = Constants.mainAction

        val pendingIntent = PendingIntent.getActivity(this, 0, notificationIntent, 0)

        val toggleOutputIntent = Intent(this, ControllerService::class.java)
        toggleOutputIntent.action = Constants.togglePowerOutput

        val stopServiceIntent = Intent(this, ControllerService::class.java)
        stopServiceIntent.action = Constants.stopForegroundAction

        return NotificationCompat.Builder(this, Constants.notificationChannelId)
            .setContentTitle("Charge Controller")
            .setContentText("Monitoring battery state")
            .setOngoing(true)
            .setContentIntent(pendingIntent)
            .addAction(0, "Toggle Output", PendingIntent.getService(this, 0, toggleOutputIntent, 0))
            .addAction(0, "Stop Monitoring", PendingIntent.getService(this, 0, stopServiceIntent, 0))
            .setSmallIcon(R.drawable.ic_charging)
            .setChannelId(Constants.notificationChannelId)
            .build()
    }

    private fun registerBroadcastReceiver() {
        registerReceiver(batteryChangeBroadcastReceiver, IntentFilter(Intent.ACTION_BATTERY_CHANGED))
    }

    private fun sendStatusUpdate() {
        val intent = Intent(Constants.controllerStatusChanged)
        intent.putExtra("output_enabled", chargeManager.outputEnabled)
        intent.putExtra("device_responsive", chargeManager.deviceResponsive)
        intent.putExtra("service_active", serviceActive)
        if (!LocalBroadcastManager.getInstance(this).sendBroadcast(intent)) {
            Log.w(tag, "Failed to send broadcast")
        }
    }

    private fun registerPreferenceChangeListener() {
        val prefs = baseContext.getSharedPreferences(Constants.sharedPreferencesName, 0)

        prefs.registerOnSharedPreferenceChangeListener { sharedPreferences, key ->
            run {
                when (key) {
                    Constants.chargeLowerLimitKey -> {
                        chargeManager.chargeLowerLimit =
                                sharedPreferences.getInt(Constants.chargeLowerLimitKey, Constants.defaultChargeLowerLimit)
                    }

                    Constants.chargeUpperLimitKey -> {
                        chargeManager.chargeUpperLimit =
                                sharedPreferences.getInt(Constants.chargeUpperLimitKey, Constants.defaultChargeUpperLimit)
                    }
                }
            }
        }

        chargeManager.chargeLowerLimit = prefs.getInt(Constants.chargeLowerLimitKey, Constants.defaultChargeLowerLimit)
        chargeManager.chargeUpperLimit = prefs.getInt(Constants.chargeUpperLimitKey, Constants.defaultChargeUpperLimit)
    }

    companion object {
        const val tag = "ForegroundService"
    }
}

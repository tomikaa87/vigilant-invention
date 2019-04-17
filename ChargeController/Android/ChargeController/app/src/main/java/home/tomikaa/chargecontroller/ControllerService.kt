package home.tomikaa.chargecontroller

import android.app.Notification
import android.app.NotificationManager
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import androidx.core.app.NotificationCompat
import androidx.localbroadcastmanager.content.LocalBroadcastManager
import androidx.lifecycle.LifecycleService
import android.util.Log
import androidx.lifecycle.Observer

class ControllerService : LifecycleService() {

    private var notificationBuilder: NotificationCompat.Builder? = null
    private var notificationManager: NotificationManager? = null
    private val batteryChangeBroadcastReceiver = BatteryChangeBroadcastReceiver()
    private val chargeManager = ChargeManager()
    private var serviceActive = false
    private var preferencesChangeListenerRegistered = false

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        super.onStartCommand(intent, flags, startId)

        when (intent?.action) {
            Constants.startForegroundAction -> {
                if (!serviceActive) {
                    Log.i(tag, "StartAction")
                    if (notificationManager == null) {
                        notificationManager = getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager

                        chargeManager.currentChargeLevel.observe(this, Observer {
                            updateNotification()
                        })

                        chargeManager.pluggedIn.observe(this, Observer {
                            updateNotification()
                        })

                        chargeManager.outputEnabled.observe(this, Observer {
                            updateNotification()
                        })
                    }

                    if (notificationBuilder == null)
                        notificationBuilder = createNotificationBuilder()

                    if (!preferencesChangeListenerRegistered)
                        registerPreferenceChangeListener()

                    registerBroadcastReceiver()
                    startForeground(Constants.notificationId, notificationBuilder?.build())
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

        Log.d(tag, "Created")

        batteryChangeBroadcastReceiver.callback = chargeManager::handleBatteryChanged
        chargeManager.changedCallback = this::sendStatusUpdate
    }

    private fun createNotificationBuilder(): NotificationCompat.Builder {
        val notificationIntent = Intent(this, MainActivity::class.java)
        notificationIntent.flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
        notificationIntent.action = Constants.mainAction

        val pendingIntent = PendingIntent.getActivity(this, 0, notificationIntent, 0)

        val toggleOutputIntent = Intent(this, ControllerService::class.java)
        toggleOutputIntent.action = Constants.togglePowerOutput

        val stopServiceIntent = Intent(this, ControllerService::class.java)
        stopServiceIntent.action = Constants.stopForegroundAction

        val builder = NotificationCompat.Builder(this, Constants.notificationChannelId)

        return builder
            .setContentTitle("Charge Controller")
            .setContentText("Monitoring battery state")
            .setOngoing(true)
            .setContentIntent(pendingIntent)
            .addAction(0, "Toggle Output", PendingIntent.getService(this, 0, toggleOutputIntent, 0))
            .addAction(0, "Stop Monitoring", PendingIntent.getService(this, 0, stopServiceIntent, 0))
            .setSmallIcon(R.drawable.ic_charging)
            .setChannelId(Constants.notificationChannelId)
            .setUsesChronometer(true)
            .setOnlyAlertOnce(true)
    }

    private fun updateNotification() {
        if (chargeManager.currentChargeLevel.value == null)
            return

        val title = if (chargeManager.pluggedIn.value == true) {
            val distance = chargeManager.currentChargeLevel.value!! - chargeManager.chargeUpperLimit

            if (chargeManager.currentChargeLevel.value!! < chargeManager.chargeUpperLimit)
                "Charging will stop at ${chargeManager.chargeUpperLimit}% ($distance%)"
            else
                "(!) Charging should have stopped at ${chargeManager.chargeUpperLimit}% (+$distance%)"
        } else {
            val distance = chargeManager.currentChargeLevel.value!! - chargeManager.chargeLowerLimit

            if (chargeManager.currentChargeLevel.value!! > chargeManager.chargeLowerLimit)
                "Charging will start at ${chargeManager.chargeLowerLimit}% (+$distance%)"
            else
                "(!) Charging should have started at ${chargeManager.chargeLowerLimit}% ($distance%)"
        }

        val text = "Output is ${if (chargeManager.outputEnabled.value == true) "enabled" else "disabled"}"

        notificationBuilder?.setContentTitle(title)
        notificationBuilder?.setContentText(text)

        val notification = notificationBuilder?.build()
        notification?.flags = Notification.FLAG_ONGOING_EVENT

        notificationManager?.notify(Constants.notificationId, notificationBuilder?.build())
    }

    private fun registerBroadcastReceiver() {
        registerReceiver(batteryChangeBroadcastReceiver, IntentFilter(Intent.ACTION_BATTERY_CHANGED))
    }

    private fun sendStatusUpdate() {
        val intent = Intent(Constants.controllerStatusChanged)
        intent.putExtra("output_enabled", chargeManager.outputEnabled.value == true)
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

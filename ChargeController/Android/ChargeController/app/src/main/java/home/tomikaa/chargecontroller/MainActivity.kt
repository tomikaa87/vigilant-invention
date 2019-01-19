package home.tomikaa.chargecontroller

import android.app.NotificationChannel
import android.app.NotificationManager
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.Observer
import android.content.*
import android.os.Build
import android.os.Bundle
import androidx.localbroadcastmanager.content.LocalBroadcastManager
import androidx.appcompat.app.AppCompatActivity
import android.view.View
import android.widget.ProgressBar
import android.widget.SeekBar
import android.widget.Switch
import android.widget.TextView
import com.google.android.material.snackbar.Snackbar

class MainActivity : AppCompatActivity() {

    class ChargeControllerBroadcastReceiver : BroadcastReceiver() {
        val outputEnabled: MutableLiveData<Boolean> by lazy {
            MutableLiveData<Boolean>()
        }

        val deviceResponsive = MutableLiveData<Boolean>()

        val serviceActive: MutableLiveData<Boolean> by lazy {
            MutableLiveData<Boolean>()
        }

        init {
            deviceResponsive.value = true
        }

        override fun onReceive(context: Context?, intent: Intent?) {
            if (intent != null) {
                if (intent.hasExtra("device_responsive"))
                    deviceResponsive.value = intent.getBooleanExtra("device_responsive", false)

                if (intent.hasExtra("output_enabled"))
                    outputEnabled.value = intent.getBooleanExtra("output_enabled", false)

                if (intent.hasExtra("service_active"))
                    serviceActive.value = intent.getBooleanExtra("service_active", false)
            }
        }
    }

    private val chargeControllerBroadcastReceiver = ChargeControllerBroadcastReceiver()
    private val batteryChangeBroadcastReceiver = BatteryChangeBroadcastReceiver()
    private var deviceUnreachableWarningShown = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        createNotificationChannel()

        setTheme(R.style.ThemeOverlay_AppCompat_Dark)

        setContentView(R.layout.activity_main)

        updateSwitchLabels(findViewById(R.id.controllerServiceSwitch), findViewById(R.id.controllerServiceSwitchLabel))
        updateSwitchLabels(findViewById(R.id.powerOutputSwitch), findViewById(R.id.powerOutputSwitchLabel))

        observeChargeController()

        batteryChangeBroadcastReceiver.callback = this::handleBatteryChanged

        androidx.localbroadcastmanager.content.LocalBroadcastManager.getInstance(this).registerReceiver(chargeControllerBroadcastReceiver, IntentFilter(Constants.controllerStatusChanged))
        registerReceiver(batteryChangeBroadcastReceiver, IntentFilter(Intent.ACTION_BATTERY_CHANGED))

        var seekBar = findViewById<SeekBar>(R.id.chargeLowerLimitSeekBar)
        seekBar?.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onStartTrackingTouch(seekBar: SeekBar?) {}
            override fun onStopTrackingTouch(seekBar: SeekBar?) {}

            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                onChargeLowerLimitSeekBarChanged(progress, fromUser)
            }
        })

        seekBar = findViewById(R.id.chargeUpperLimitSeekBar)
        seekBar?.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onStartTrackingTouch(seekBar: SeekBar?) {}
            override fun onStopTrackingTouch(seekBar: SeekBar?) {}

            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                onChargeUpperLimitSeekBarChanged(progress, fromUser)
            }
        })

        loadPreferences()

        controlService(true)
        requestServiceStatusUpdate()
    }

    private fun observeChargeController() {
        chargeControllerBroadcastReceiver.outputEnabled.observe(this, Observer<Boolean> {
            val switch = findViewById<Switch>(R.id.powerOutputSwitch)
            switch?.isChecked = it!!
            updateSwitchLabels(switch, findViewById(R.id.powerOutputSwitchLabel))
        })

        chargeControllerBroadcastReceiver.serviceActive.observe(this, Observer<Boolean> {
            val switch = findViewById<Switch>(R.id.controllerServiceSwitch)
            switch?.isChecked = it!!
            updateSwitchLabels(switch, findViewById(R.id.controllerServiceSwitchLabel))
        })

        chargeControllerBroadcastReceiver.deviceResponsive.observe(this, Observer<Boolean> {
            when (it) {
                true -> {
                    if (deviceUnreachableWarningShown) {
                        Snackbar.make(findViewById(R.id.mainLayout), "Remote device is reachable again", 3000).show()
                        deviceUnreachableWarningShown = false
                    }
                }

                false -> {
                    deviceUnreachableWarningShown = true
                    Snackbar.make(findViewById(R.id.mainLayout), "Remote device is unreachable", 3000).show()
                }
            }
        })
    }

    override fun onDestroy() {
        androidx.localbroadcastmanager.content.LocalBroadcastManager.getInstance(this).unregisterReceiver(batteryChangeBroadcastReceiver)
        unregisterReceiver(batteryChangeBroadcastReceiver)

        super.onDestroy()
    }

    fun onControllerServiceSwitchClicked(view: View) {
        val switch = view as Switch

        updateSwitchLabels(switch, findViewById(R.id.controllerServiceSwitchLabel))
        controlService(switch.isChecked)
    }

    fun onPowerOutputSwitchClicked(view: View) {
        val switch = view as Switch

        updateSwitchLabels(switch, findViewById(R.id.powerOutputSwitchLabel))

        val intent = Intent(this, ControllerService::class.java)
        intent.action = if (switch.isChecked) Constants.enablePowerOutput else Constants.disablePowerOutput
        startService(intent)
    }

    fun onChargeLowerLimitSeekBarChanged(progress: Int, fromUser: Boolean) {
        val upperLimitSeekBar = findViewById<SeekBar>(R.id.chargeUpperLimitSeekBar)
        upperLimitSeekBar?.min = progress

        val indicator = findViewById<TextView>(R.id.chargeLowerLimitIndicator)
        indicator?.text = "$progress%"

        if (fromUser) {
            val prefs = baseContext.getSharedPreferences(Constants.sharedPreferencesName, 0)
            prefs.edit().putInt(Constants.chargeLowerLimitKey, progress).apply()
        }
    }

    fun onChargeUpperLimitSeekBarChanged(progress: Int, fromUser: Boolean) {
        val lowerLimitSeekBar = findViewById<SeekBar>(R.id.chargeLowerLimitSeekBar)
        lowerLimitSeekBar?.max = progress

        val indicator = findViewById<TextView>(R.id.chargeUpperLimitIndicator)
        indicator.text = "$progress%"

        if (fromUser) {
            val prefs = baseContext.getSharedPreferences(Constants.sharedPreferencesName, 0)
            prefs.edit().putInt(Constants.chargeUpperLimitKey, progress).apply()
        }
    }

    private fun updateSwitchLabels(switch: Switch?, textView: TextView?) {
        if (switch != null && textView != null) {
            textView.text = if (switch.isChecked) switch.textOn else switch.textOff
        }
    }

    private fun requestServiceStatusUpdate() {
        val intent = Intent(this, ControllerService::class.java)
        intent.action = Constants.sendStatusUpdate
        startService(intent)
    }

    private fun createNotificationChannel() {
        // Create the NotificationChannel, but only on API 26+ because
        // the NotificationChannel class is new and not in the support library
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val name = getString(R.string.channel_name)
            val descriptionText = getString(R.string.channel_description)
            val importance = NotificationManager.IMPORTANCE_DEFAULT
            val channel = NotificationChannel(Constants.notificationChannelId, name, importance).apply {
                description = descriptionText
            }
            // Register the channel with the system
            val notificationManager: NotificationManager =
                getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
            notificationManager.createNotificationChannel(channel)
        }
    }

    private fun handleBatteryChanged(level: Int, pluggedIn: Boolean) {
        val indicator = findViewById<TextView>(R.id.batteryLevelIndicator)
        indicator?.text = level.toString()

        val progress = findViewById<ProgressBar>(R.id.progressBar)
        progress.progress = level
    }

    private fun loadPreferences() {
        val sharedPreferences = baseContext.getSharedPreferences(Constants.sharedPreferencesName, 0)

        findViewById<SeekBar>(R.id.chargeLowerLimitSeekBar)?.progress =
                sharedPreferences.getInt(Constants.chargeLowerLimitKey, Constants.defaultChargeLowerLimit)
        findViewById<SeekBar>(R.id.chargeUpperLimitSeekBar)?.progress =
                sharedPreferences.getInt(Constants.chargeUpperLimitKey, Constants.defaultChargeUpperLimit)
    }

    private fun controlService(start: Boolean) {
        val intent = Intent(this, ControllerService::class.java)
        intent.action = if (start) Constants.startForegroundAction else Constants.stopForegroundAction
        startService(intent)
    }
}

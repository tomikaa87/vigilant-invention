package home.tomikaa.chargecontroller

class Constants {
    companion object {
        const val startForegroundAction = "home.tomikaa.foregroundservice.action.start"
        const val stopForegroundAction = "home.tomikaa.foregroundservice.action.stop"
        const val enablePowerOutput = "home.tomikaa.foregroundservice.action.enable_output"
        const val disablePowerOutput = "home.tomikaa.foregroundservice.action.disable_outout"
        const val togglePowerOutput = "home.tomikaa.foregroundservice.action.toggle_output"
        const val sendStatusUpdate = "home.tomikaa.foregroundservice.action.status_update"

        const val mainAction = "home.tomikaa.foregroundservice.action.main"

        const val controllerStatusChanged = "home.tomikaa.mainactivity.action.controller_status_changed"

        const val notificationId = 101
        const val notificationChannelId = "home.tomikaa.foregroundservice.notification.channel.default"

        const val sharedPreferencesName = "home.tomikaa.chargercontroller.shared_prefs"
        const val chargeLowerLimitKey = "ChargeLowerLimit"
        const val chargeUpperLimitKey = "ChargeUpperLimit"

        const val defaultChargeLowerLimit = 70
        const val defaultChargeUpperLimit = 80
    }
}
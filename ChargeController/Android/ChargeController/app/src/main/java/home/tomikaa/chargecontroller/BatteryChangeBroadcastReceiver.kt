package home.tomikaa.chargecontroller

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.os.BatteryManager

class BatteryChangeBroadcastReceiver : BroadcastReceiver() {
    var callback: ((level: Int, pluggedIn: Boolean) -> Unit)? = null

    override fun onReceive(context: Context?, intent: Intent?) {
        if (intent == null)
            return

        callback?.invoke(
            intent.getIntExtra(BatteryManager.EXTRA_LEVEL, 0),
            intent.getIntExtra(BatteryManager.EXTRA_PLUGGED, 0) != 0)
    }
}
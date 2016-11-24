package com.stuffomatic.asgp.receiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;

public class EnableExtraPlungers extends BroadcastReceiver
{
    public static String BEGIN = "begin-date";
    public static String DURATION = "duration";

    public void onReceive( Context context, Intent intent ) {

        final long begin = intent.getLongExtra( BEGIN, 0 );
        final long duration = intent.getLongExtra( DURATION, 0 );
        
        final SharedPreferences preferences =
            context.getSharedPreferences( "power-up", Context.MODE_PRIVATE );

        final Editor editor = preferences.edit();
        editor.putLong( "extra-plungers-begin", begin );
        editor.putLong( "extra-plungers-end", begin + duration );
        editor.commit();
    }
};

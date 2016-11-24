package com.stuffomatic.notification;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

import java.util.HashMap;
import java.util.Map;

import com.stuffomatic.event.EventManager;

public class Publisher extends BroadcastReceiver
{
    public static String NOTIFICATION_ID = "notification-id";
    public static String NOTIFICATION = "notification";

    public void onReceive( Context context, Intent intent ) {

        final NotificationManager notificationManager =
            ( NotificationManager ) context.getSystemService
            ( Context.NOTIFICATION_SERVICE );

        final Notification notification =
            intent.getParcelableExtra( NOTIFICATION );
        final int id = intent.getIntExtra( NOTIFICATION_ID, 0 );
        
        final Map< String, String > properties = new HashMap<>();
        properties.put( "notification-id", String.valueOf( id ) );
        EventManager.tagEvent( "notification-notified", properties );
        
        notificationManager.notify( id, notification );
    }
}

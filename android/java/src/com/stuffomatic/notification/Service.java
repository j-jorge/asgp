package com.stuffomatic.notification;

import android.app.Activity;
import android.app.AlarmManager;
import android.app.Notification;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.graphics.drawable.BitmapDrawable;
import android.os.SystemClock;
import android.support.v4.app.NotificationCompat;

public class Service
{
    private final Activity mActivity;
    private final Class<?> mIntentClass;
    private final Class<?> mDeleteIntentClass;
    
    public Service
        ( Activity activity, Class<?> intentClass,
          Class<?> deleteIntentClass ) {
        
        mActivity = activity;
        mIntentClass = intentClass;
        mDeleteIntentClass = deleteIntentClass;
    }
    
    public void scheduleNotification
        ( String title, String message, int intentId, int notificationId,
          long delay, long repeatInterval, int smallIcon, int largeIcon ) {

        final PendingIntent pendingIntent =
            buildIntent
            ( title, message, intentId, notificationId, smallIcon, largeIcon );

        final long displayDateMs = SystemClock.elapsedRealtime() + delay;
        final AlarmManager alarmManager =
            ( AlarmManager ) mActivity.getSystemService
            ( Context.ALARM_SERVICE );
        alarmManager.setInexactRepeating
            ( AlarmManager.ELAPSED_REALTIME_WAKEUP, displayDateMs,
              repeatInterval, pendingIntent );
    }
    
    public void scheduleNotification
        ( String title, String message, int intentId, int notificationId,
          long delay, int smallIcon, int largeIcon ) {

        final PendingIntent pendingIntent =
            buildIntent
            ( title, message, intentId, notificationId, smallIcon, largeIcon );

        final long displayDateMs = SystemClock.elapsedRealtime() + delay;
        final AlarmManager alarmManager =
            ( AlarmManager ) mActivity.getSystemService
            ( Context.ALARM_SERVICE );
        alarmManager.set
            ( AlarmManager.ELAPSED_REALTIME_WAKEUP, displayDateMs,
              pendingIntent );
     }

    private PendingIntent buildIntent
        ( String title, String message, int intentId, int notificationId,
          int smallIcon, int largeIcon ) {

        final Notification notification =
            buildNotification( title, message, notificationId, smallIcon,
                               largeIcon );

        final Intent notificationIntent =
            new Intent( mActivity, Publisher.class );
        notificationIntent.putExtra
            ( Publisher.NOTIFICATION_ID, intentId );
        notificationIntent.putExtra( Publisher.NOTIFICATION, notification );

        return PendingIntent.getBroadcast
            ( mActivity, intentId, notificationIntent,
              PendingIntent.FLAG_CANCEL_CURRENT );
    }
    
    private Notification buildNotification
        ( String title, String message, int notificationId, int smallIcon,
          int largeIcon ) {

        NotificationCompat.BigTextStyle textStyle =
            new NotificationCompat.BigTextStyle();

        textStyle.setBigContentTitle( title );
        textStyle.bigText( message );

        final NotificationCompat.Builder builder =
            new NotificationCompat.Builder( mActivity )
            .setStyle( textStyle )
            .setContentTitle( title )
            .setContentText( message )
            .setContentIntent( createContentIntent( notificationId ) )
            .setDeleteIntent( createDeleteIntent( notificationId ) )
            .setAutoCancel( true )
            .setSmallIcon( smallIcon )
            .setLargeIcon
            ( ( ( BitmapDrawable )
                mActivity.getResources().getDrawable( largeIcon ) )
              .getBitmap() );

        return builder.build();
    }

    private PendingIntent createContentIntent( int notificationId ) {
        
        final Intent intent = new Intent( mActivity, mIntentClass );
        intent.putExtra( "notification-id", notificationId );

        return PendingIntent.getBroadcast
            ( mActivity, 0, intent, PendingIntent.FLAG_UPDATE_CURRENT );
    }

    private PendingIntent createDeleteIntent( int notificationId ) {
        
        final Intent intent = new Intent( mActivity, mDeleteIntentClass );
        intent.putExtra( "notification-id", notificationId );

        return PendingIntent.getBroadcast
            ( mActivity, 0, intent, PendingIntent.FLAG_UPDATE_CURRENT );
    }
}

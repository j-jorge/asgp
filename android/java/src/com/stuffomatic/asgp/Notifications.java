package com.stuffomatic.asgp;

import com.stuffomatic.asgp.receiver.EnableExtraPlungers;
import com.stuffomatic.asgp.receiver.NotificationClicked;
import com.stuffomatic.asgp.receiver.NotificationDismissed;
import com.stuffomatic.event.EventManager;
import com.stuffomatic.notification.Service;

import com.stuffomatic.coasters.R;

import java.util.HashMap;
import java.util.Map;

import android.app.Activity;
import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.SystemClock;
import android.support.v4.app.NotificationManagerCompat;

class Notifications
{
    static final int mNotificationId = 1;

    static final long mOneHourMs = 60 * 60 * 1000;
    static final long mHalfDayMs = 12 * mOneHourMs;
    static final long mOneDayMs = 24 * mOneHourMs;
    static final long mThreeDaysMs = 3 * mOneDayMs;
    static final long mOneWeekMs = 7 * mOneDayMs;

    static final int mIntentId1 = 1;
    static final int mIntentId2 = 2;
    static final int mIntentId3 = 3;
    static final int mIntentId4 = 4;
    static final int mIntentIdPlungersNotification = 5;
    static final int mIntentIdSavePlungers = 6;
    
    final Service mService;
    final Activity mActivity;
    
    public Notifications( Activity activity ) {
        mService =
            new Service
            ( activity, NotificationClicked.class,
              NotificationDismissed.class );
        mActivity = activity;
    }
    
    public void scheduleNotifications() {

        tagAvailability();
        extraPlungerDates();
        scheduleInService();
    }

    private void tagAvailability() {

        if ( Build.VERSION.SDK_INT < Build.VERSION_CODES.KITKAT ) {
            return;
        }
        
        final Map< String, String > properties = new HashMap<>();
        properties.put
            ( "enabled",
              String.valueOf
              ( NotificationManagerCompat.from( mActivity )
                .areNotificationsEnabled() ) );
        EventManager.tagEvent( "notification-state", properties );
    }

    private void extraPlungerDates() {

        final long delay = mOneDayMs;

        extraPlungerPreferences( delay );
        extraPlungerNotification( delay );
    }

    private void extraPlungerPreferences( long delay ) {

        final long date = SystemClock.elapsedRealtime() + delay;
        final Intent intent =
            new Intent( mActivity, EnableExtraPlungers.class );
        intent.putExtra( EnableExtraPlungers.BEGIN, date );
        intent.putExtra( EnableExtraPlungers.DURATION, mOneHourMs );

        final PendingIntent pendingIntent =
            PendingIntent.getBroadcast
            ( mActivity, mIntentIdSavePlungers, intent,
              PendingIntent.FLAG_CANCEL_CURRENT );

        final AlarmManager alarmManager =
            ( AlarmManager ) mActivity.getSystemService
            ( Context.ALARM_SERVICE );
        alarmManager.set
            ( AlarmManager.ELAPSED_REALTIME_WAKEUP, date, pendingIntent );
    }
    
    private void extraPlungerNotification( long delay ) {

        mService.scheduleNotification
            ( mActivity.getString( R.string.notification_extra_plunger_title ),
              mActivity.getString
              ( R.string.notification_extra_plungers_message ),
              mIntentIdPlungersNotification, mNotificationId, delay,
              R.drawable.ic_notification,
              R.drawable.ic_launcher );
    }

    private void scheduleInService() {
        final int smallIcon = R.drawable.ic_notification;
        final int largeIcon = R.drawable.ic_launcher;
        
        mService.scheduleNotification
            ( mActivity.getString( R.string.notification_title_1 ),
              mActivity.getString( R.string.notification_message_1 ),
              mIntentId1, mNotificationId, mOneHourMs, smallIcon, largeIcon );
        mService.scheduleNotification
            ( mActivity.getString( R.string.notification_title_2 ),
              mActivity.getString( R.string.notification_message_2 ),
              mIntentId2, mNotificationId, mHalfDayMs, smallIcon, largeIcon );
        mService.scheduleNotification
            ( mActivity.getString( R.string.notification_title_3 ),
              mActivity.getString( R.string.notification_message_3 ),
              mIntentId3, mNotificationId, mThreeDaysMs, smallIcon, largeIcon );
        mService.scheduleNotification
            ( mActivity.getString( R.string.notification_title_4 ),
              mActivity.getString( R.string.notification_message_4 ),
              mIntentId4, mNotificationId, mOneWeekMs, mOneWeekMs, smallIcon,
              largeIcon );
    }
}

package com.stuffomatic.event;

import android.app.Application;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;

import com.amplitude.api.Amplitude;
import com.google.firebase.analytics.FirebaseAnalytics;
import net.hockeyapp.android.metrics.MetricsManager;

import org.json.JSONObject;

import java.util.Collections;
import java.util.Map;
import java.util.UUID;

public class EventManager
{
    private static FirebaseAnalytics mFirebase;

    public static void initialize
        ( Application application, String amplitudeAppId ) {

        Amplitude.getInstance().initialize( application, amplitudeAppId )
            .enableForegroundTracking( application );
        MetricsManager.register( application, application );
        mFirebase = FirebaseAnalytics.getInstance( application );
        mFirebase.setUserId( getOrCreateUserId( application ) );
    }
    
    public static void tagEvent( String tag ) {
        tagEvent( tag, Collections.< String, String >emptyMap() );
    }
    
    public static void tagEvent
        ( String tag, Map< String, String > properties ) {
        
        Amplitude.getInstance().logEvent( tag, new JSONObject( properties ) );
        MetricsManager.trackEvent( tag, properties );

        final Bundle bundle = new Bundle();
        
        for ( Map.Entry< String, String > entry : properties.entrySet() )
            bundle.putString
                ( formatFirebaseIdentifier( entry.getKey() ),
                  formatFirebaseIdentifier( entry.getValue() ) );

        mFirebase.logEvent( formatFirebaseIdentifier( tag ), bundle );
    }

    private static String getOrCreateUserId( Application application ) {

        final SharedPreferences preferences =
            PreferenceManager.getDefaultSharedPreferences( application );

        if ( preferences.contains( "coasters-user-id" ) )
            return preferences.getString( "coasters.user-id", "unset-id" );

        final String userId = UUID.randomUUID().toString();
        
        preferences
            .edit()
            .putString( "coasters-user-id", userId )
            .apply();
        
        return userId;
    }

    private static String formatFirebaseIdentifier( String value ) {

        String result = new String();

        for ( char c : value.toCharArray() ) {
            if ( ( ( '0' <= c ) && ( c <= '9' ) )
                 || ( ( 'a' <= c ) && ( c <= 'z' ) )
                 || ( ( 'A' <= c ) && ( c <= 'Z' ) )
                 || ( c == '_' ) )
                result += c;
            else
                result += '_';
        }

        return result;
    }
}

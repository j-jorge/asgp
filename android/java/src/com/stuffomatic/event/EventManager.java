package com.stuffomatic.event;

import android.app.Application;
import android.content.Context;

import com.amplitude.api.Amplitude;
import net.hockeyapp.android.metrics.MetricsManager;

import org.json.JSONObject;

import java.util.Map;

public class EventManager
{
    public static void initialize
        ( Application application, String amplitudeAppId ) {

        Amplitude.getInstance().initialize( application, amplitudeAppId )
            .enableForegroundTracking( application );
        MetricsManager.register( application, application );
    }
    
    public static void tagEvent
        ( String tag, Map< String, String > properties ) {
        
        Amplitude.getInstance().logEvent( tag, new JSONObject( properties ) );
        MetricsManager.trackEvent( tag, properties );
    }
}

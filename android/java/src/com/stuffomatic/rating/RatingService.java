package com.stuffomatic.rating;

import com.kobakei.ratethisapp.RateThisApp;

import android.R;
import android.content.Context;
import android.os.Handler;
import android.os.Looper;

public final class RatingService
{
    private static Context mContext;
    
    public static void init
        ( Context context, String url, int title, int message, int yes,
          int contact, int cancel ) {

        mContext = context;
        
        final RateThisApp.Config config = new RateThisApp.Config( 3, 10 );
        config.setUrl( url );
        config.setTitle( title );
        config.setMessage( message );
        config.setYesButtonText( yes );
        config.setNoButtonText( contact );
        config.setCancelButtonText( cancel );
        
        RateThisApp.init( config );
        RateThisApp.onStart( mContext );
    }

    public static void setListener( DialogListener listener ) {
        RateThisApp.setCallback( new CallbackGlue( listener ) );
    }
    
    public static boolean show() {

        if ( !RateThisApp.shouldShowRateDialog() )
            return false;

        final Handler handler = new Handler( Looper.getMainLooper() );

        handler.post(
            new Runnable() {
                @Override 
                public void run() {
                    RateThisApp.showRateDialog
                        ( mContext,
                          R.style.Theme_DeviceDefault_Light_Dialog_Alert );
                }
            }
        );

        return true;
    }
}

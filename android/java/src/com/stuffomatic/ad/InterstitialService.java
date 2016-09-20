package com.stuffomatic.ad;

import com.stuffomatic.jni.NativeCallBridge;

import com.amplitude.api.Amplitude;

import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.InterstitialAd;
import android.content.Context;
import android.os.Handler;
import android.os.Looper;

public class InterstitialService
{
    private InterstitialAd mInterstitialAd;
    private long mCallback = -1;
    
    public InterstitialService( Context context ) {

        mInterstitialAd = new InterstitialAd( context );
        mInterstitialAd.setAdUnitId("ca-app-pub-8517534578027121/5449175693");

        mInterstitialAd.setAdListener( new AdListener() {
            @Override
            public void onAdClosed() {
                adClosed();
            }
        } );

        requestNewInterstitial();
    }

    public void show( long callback ) {

        assert( mCallback == -1 );
        assert( callback != -1 );

        mCallback = callback;
        
        final Handler handler = new Handler( Looper.getMainLooper() );

        handler.post(
            new Runnable() {
                @Override 
                public void run() {
                    tryDisplay();
                }
            }
        );
    }

    private void tryDisplay() {
        
        if (mInterstitialAd.isLoaded()) {
            Amplitude.getInstance().logEvent( "ad-shown" );
            mInterstitialAd.show();
        } else {
            Amplitude.getInstance().logEvent( "ad-none" );
            dispatchClosed();
            requestNewInterstitial();
        }
    }

    private void adClosed() {
        
        Amplitude.getInstance().logEvent( "ad-closed" );
        requestNewInterstitial();
        dispatchClosed();
    }

    private void dispatchClosed() {
        final long callback = mCallback;
        mCallback = -1;
        NativeCallBridge.call( callback );
    }

    private void requestNewInterstitial() {

        final AdRequest adRequest = new AdRequest.Builder().build();
        mInterstitialAd.loadAd( adRequest );
    }
}

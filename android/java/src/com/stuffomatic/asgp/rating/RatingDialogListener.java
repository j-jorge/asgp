package com.stuffomatic.asgp.rating;

import com.stuffomatic.event.EventManager;
import com.stuffomatic.rating.DialogListener;

import net.hockeyapp.android.FeedbackManager;

import android.app.Activity;

public class RatingDialogListener implements DialogListener
{
    private final Activity mActivity;
    
    public RatingDialogListener( Activity activity ) {
        mActivity = activity;
    }
    
    @Override
    public void onShow() {
        EventManager.tagEvent( "rate-dialog-shown" );
    }
    
    @Override
    public void onRate() {
        EventManager.tagEvent( "rate-dialog-yes" );
    }
        
    @Override
    public void onLater() {
        EventManager.tagEvent( "rate-dialog-later" );
    }
    
    @Override
    public void onContact() {
        EventManager.tagEvent( "rate-dialog-contact" );
        FeedbackManager.showFeedbackActivity( mActivity );
    }
}


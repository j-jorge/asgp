package com.stuffomatic.rating;

import com.kobakei.ratethisapp.RateThisApp;

class CallbackGlue implements RateThisApp.Callback
{
    private final DialogListener mListener;

    public CallbackGlue( DialogListener listener ) {
        mListener = listener;
    }

    @Override
    public void onYesClicked() {
        mListener.onRate();
    }

    @Override
    public void onNoClicked() {
        mListener.onContact();
    }

    @Override
    public void onCancelClicked() {
        mListener.onLater();
    }

    @Override
    public void onShow() {
        mListener.onShow();
    }
}

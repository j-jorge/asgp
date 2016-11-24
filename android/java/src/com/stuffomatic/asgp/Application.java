package com.stuffomatic.asgp;

import com.stuffomatic.event.EventManager;
import com.stuffomatic.coasters.R;

public class Application extends android.app.Application {

    @Override
    public void onCreate() 
    {
        super.onCreate();

        EventManager.initialize
            ( this, getString( R.string.amplitude_app_id ) );
    }
}

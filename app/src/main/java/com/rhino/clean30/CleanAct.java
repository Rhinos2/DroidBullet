package com.rhino.clean30;

import android.app.Activity;
import android.os.Bundle;


public class CleanAct extends Activity
{  
	GL2View mView;
	
    @Override protected void onCreate(Bundle icicle)
    {    
    	super.onCreate( icicle );
    	System.loadLibrary( "testBullet" );
        mView = new GL2View( getApplication() );
        setContentView( mView );
    }   
    
    @Override protected void onResume()
    {
        super.onResume();
        mView.onResume();
    }    

}

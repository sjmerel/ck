package com.crickettechnology.samples.DspTouch;

import android.os.Bundle;
import android.os.Handler;
import android.app.Activity;
import android.graphics.Point;
import android.view.Menu;
import android.view.MotionEvent;

public class DspTouchActivity extends Activity 
{

	@Override
	protected void onCreate(Bundle savedInstanceState) 
    {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_dsptouch);

        getWindowManager().getDefaultDisplay().getSize(m_size);

        dspTouchInit();
        m_handler.post(m_task);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) 
    {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.dsp_touch, menu);
		return true;
	}

    @Override
    public void onPause()
    {
        super.onPause();
        dspTouchSuspend();
    }

    @Override
    public void onResume()
    {
        super.onResume();
        dspTouchResume();
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
        m_handler.removeCallbacks(m_task);
        dspTouchShutdown();
    }

    @Override
    public boolean dispatchTouchEvent(MotionEvent event)
    {
        switch (event.getActionMasked())
        {
            case MotionEvent.ACTION_DOWN:
                setParams(event);
                dspTouchStartEffect();
                break;

            case MotionEvent.ACTION_MOVE:
                setParams(event);
                break;

            case MotionEvent.ACTION_CANCEL:
            case MotionEvent.ACTION_UP:
                dspTouchStopEffect();
                break;
        }

        return super.dispatchTouchEvent(event);
    }

    ////////////////////////////////////////

    private Point m_size = new Point();
    private Handler m_handler = new Handler();
    private Runnable m_task = new Runnable()
    {
        public void run()
        {
            dspTouchUpdate();
            m_handler.postDelayed(this, 30);
        }
    };


    ////////////////////////////////////////

    private void setParams(MotionEvent event)
    {
        dspTouchSetEffectParams(event.getX()/m_size.x, (m_size.y - event.getY())/m_size.y);
    }

    ////////////////////////////////////////

    static
    {
        // load our native library
        System.loadLibrary("dsptouch");
    }

    ////////////////////////////////////////

    private native void dspTouchInit();
    private native void dspTouchUpdate();
    private native void dspTouchShutdown();
    private native void dspTouchSuspend();
    private native void dspTouchResume();
    private native void dspTouchStartEffect();
    private native void dspTouchSetEffectParams(float x, float y);
    private native void dspTouchStopEffect();

}

package com.crickettechnology.game;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;

class GameGLSurfaceView extends GLSurfaceView 
{
    public GameGLSurfaceView(Activity activity) 
    {
        super(activity);
        m_renderer = new GameRenderer();
        setRenderer(m_renderer);
    }

    public boolean onTouchEvent(final MotionEvent event) 
    {
        int action = event.getAction();
        int actionCode = -1;
        switch (action)
        {
            case MotionEvent.ACTION_DOWN:
                actionCode = 0;
                break;

            case MotionEvent.ACTION_MOVE:
                actionCode = 1;
                break;

            case MotionEvent.ACTION_UP:
                actionCode = 2;
                break;
        }

        if (actionCode >= 0)
        {
            nativeTouch(actionCode, event.getX(), event.getY());
        }

        return true;
    }

    private GameRenderer m_renderer;

    private static native void nativeTouch(int code, float x, float y);
}


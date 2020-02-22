package com.crickettechnology.game;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.os.Bundle;

public class GameActivity extends Activity 
{
    public void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        m_glView = new GameGLSurfaceView(this);
        setContentView(m_glView);
        nativeCreate(this);
    }

    protected void onPause() 
    {
        super.onPause();
        m_glView.onPause();
        nativePause();
    }

    protected void onResume() 
    {
        super.onResume();
        m_glView.onResume();
        nativeResume();
    }

    protected void onDestroy() 
    {
        nativeDestroy();
        super.onDestroy();
    }

    private GLSurfaceView m_glView;

    private static native void nativeCreate(Activity activity);
    private static native void nativePause();
    private static native void nativeResume();
    private static native void nativeDestroy();

    static 
    {
        System.loadLibrary("game");
    }
}


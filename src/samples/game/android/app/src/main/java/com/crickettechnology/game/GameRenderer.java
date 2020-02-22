package com.crickettechnology.game;

import android.opengl.GLSurfaceView;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

class GameRenderer implements GLSurfaceView.Renderer 
{
    public void onSurfaceCreated(GL10 gl, EGLConfig config) 
    {
        nativeGlInit();
    }

    public void onSurfaceChanged(GL10 gl, int w, int h) 
    {
        nativeResize(w, h);
    }

    public void onDrawFrame(GL10 gl) 
    {
        nativeRender();
    }

    private static native void nativeGlInit();
    private static native void nativeResize(int w, int h);
    private static native void nativeRender();
}


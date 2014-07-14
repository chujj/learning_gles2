package opengl.livewallpaper.cube;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.content.res.AssetManager;

import com.android.gl2jni.AssetHelper;
import com.android.gl2jni.GL2JNILib;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;

public class CubeWallpaperService extends OpenGLES2WallpaperService {

	@Override
	Renderer getNewRenderer() {
//		return new CubeRenderer();
		return new MyRenderer();
	}
	
	
	private class MyRenderer implements GLSurfaceView.Renderer {
		private int mNativeContext;

		public void onDrawFrame(GL10 gl) {
			if (mNativeContext != 0)
				GL2JNILib.step(mNativeContext);
		}

		public void onSurfaceChanged(GL10 gl, int width, int height) {
			if (mNativeContext != 0) {
				GL2JNILib.eglChange(mNativeContext, width, height);
			}
		}

		public void onSurfaceCreated(GL10 gl, EGLConfig config) {
			AssetManager am = CubeWallpaperService.this.getAssets();
			Context ctx = CubeWallpaperService.this;
			String obj = AssetHelper.copyAssetToSDRAM(ctx , am, "earth_universe.obj");
			String tex_png = AssetHelper.copyAssetToSDRAM(ctx , am, "earth_universe.png");
			String vert = AssetHelper.copyAssetToSDRAM(ctx , am, "vert.vert");
			String frag = AssetHelper.copyAssetToSDRAM(ctx , am, "frag.frag");
			mNativeContext = GL2JNILib.init(obj, tex_png, vert, frag);
		}
	}
	
	
}

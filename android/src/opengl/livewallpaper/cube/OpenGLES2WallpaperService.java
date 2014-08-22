package opengl.livewallpaper.cube;

import com.umeng.analytics.MobclickAgent;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView.Renderer;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;

public abstract class OpenGLES2WallpaperService extends GLWallpaperService {

	@Override
	public Engine onCreateEngine() {
		return new OpenGLES2Engine();
	}

	class OpenGLES2Engine extends GLWallpaperService.GLEngine {
		
		public final static String ENGINE_TAG = "ENGINE";

		@Override
		public void onVisibilityChanged(boolean visible) {
			// ZHUJJ Auto-generated method stub
			super.onVisibilityChanged(visible);
			if (visible) {
				MobclickAgent.onResume(OpenGLES2WallpaperService.this);
			} else {
				MobclickAgent.onPause(OpenGLES2WallpaperService.this);
			}
		}
		@Override
		public void onCreate(SurfaceHolder surfaceHolder) {
			super.onCreate(surfaceHolder);
			// Check if the system supports OpenGL ES 2.0.
			final ActivityManager activityManager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);

			final ConfigurationInfo configurationInfo = activityManager
					.getDeviceConfigurationInfo();

			final boolean supportsEs2 = configurationInfo.reqGlEsVersion >= 0x20000;
			if (supportsEs2) {
				// Request an OpenGL ES 2.0 compatible context.
				setEGLContextClientVersion(2);
				// On Honeycomb+ devices, this improves the performance when
				// leaving and resuming the live wallpaper.
				setPreserveEGLContextOnPause(true);
				// Set the renderer to our user-defined renderer.
				Renderer render = getNewRenderer();
				if (render instanceof CommandListener) {
					mCommandListener = (CommandListener) render;
				}
				setRenderer(render);
			} else {
				throw new RuntimeException("only OpenGL ES 2.0 support yet");
				// This is where you could create an OpenGL ES 1.x compatible
				// renderer if you wanted to support both ES 1 and ES 2.
//				return;
			}
		}

		private CommandListener mCommandListener;
		@Override
		public Bundle onCommand(String action, int x, int y, int z,
				Bundle extras, boolean resultRequested) {
			
			if (mCommandListener != null && mCommandListener.isAvaliable()) {
				mCommandListener.onComand(action, x, y, z, extras, resultRequested);
			}
			
			return super.onCommand(action, x, y, z, extras, resultRequested);
		}
		
		
	}

	abstract Renderer getNewRenderer();
}

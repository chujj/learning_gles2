package opengl.livewallpaper.cube;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.service.wallpaper.WallpaperService;
import android.view.SurfaceHolder;

public abstract class GLWallpaperService extends WallpaperService {

	public class GLEngine extends Engine {
		private static final String TAG = "GLEngine";
		
		private WallpaperGLSurfaceView glSurfaceView;
		private boolean rendererHasBeenSet;
		
		@Override
		public void onCreate(SurfaceHolder surfaceHolder) {
			super.onCreate(surfaceHolder);
			glSurfaceView = new WallpaperGLSurfaceView(GLWallpaperService.this);
		}
		
		@Override
		public void onVisibilityChanged(boolean visible) {
			super.onVisibilityChanged(visible);
			if (rendererHasBeenSet) {
				if (visible) {
					glSurfaceView.onResume();
				} else {
					glSurfaceView.onPause();
				}
			}
		}

		@Override
		public void onDestroy() {
			super.onDestroy();
			glSurfaceView.onDestroy();
		}
		
		protected void setRenderer(Renderer renderer) {
			glSurfaceView.setRenderer(renderer);
			rendererHasBeenSet = true;
		}

		protected void setEGLContextClientVersion(int version) {
			glSurfaceView.setEGLContextClientVersion(version);
		}
		
		protected void setPreserveEGLContextOnPause(boolean preserve) {
			// ZHUJJ must abrove honeycomb, so ignore it
//			glSurfaceView.setPreserveEGLContextOnPause(preserve);
		}

		public class WallpaperGLSurfaceView extends GLSurfaceView {
			private static final String TAG = "WallpaperGLSurfaceView";

			WallpaperGLSurfaceView(Context context) {
				super(context);
			}

			@Override
			public SurfaceHolder getHolder() {
				return getSurfaceHolder();
			}

			public void onDestroy() {
				super.onDetachedFromWindow();
			}
		}
	}
}

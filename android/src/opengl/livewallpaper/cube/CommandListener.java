package opengl.livewallpaper.cube;

import android.os.Bundle;

public interface CommandListener {

	boolean isAvaliable();

	void onComand(String action, int x, int y, int z, Bundle extras,
			boolean resultRequested);

}

package com.android.gl2jni;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;

import android.content.Context;
import android.content.res.AssetManager;
import android.widget.Toast;

public class AssetHelper {
	public static String copyAssetToSDRAM(Context context, AssetManager assetManager, String strFilename) {
		File fileTarget  = null;
		try {
			// complete path to target file
			fileTarget = new File(context.getFilesDir(), strFilename);

			// data source stream
			InputStream istr = assetManager.open(strFilename);

			// data destination stream
			// NOTE: at this point you'll get an exception if you don't have
			// permission to access SDRAM ! (see manifest)
			OutputStream ostr = new FileOutputStream(fileTarget);

			byte[] buffer = new byte[1024];
			int length;
			while ((length = istr.read(buffer)) > 0) {
				ostr.write(buffer, 0, length);
			}
			ostr.flush();
			ostr.close();
			istr.close();

		} catch (Exception e) {
			Toast.makeText(context, "File-Copy Error: " + strFilename,
					Toast.LENGTH_LONG).show();
			e.printStackTrace();
		}

		if (fileTarget == null) {
			return null;
		} else {
			return fileTarget.getAbsolutePath();
		}
	}
}

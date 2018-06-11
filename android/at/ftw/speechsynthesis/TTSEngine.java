/**
 * SALB TTS
 * @author Markus Toman, 2015
 */

package at.ftw.speechsynthesis;

import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;

import android.content.Context;
import android.content.res.AssetManager;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;

/**
 * The TTS Engine wraps native code for HMM-based speech synthesis.
 */
public class TTSEngine {

	private static final int MODEL_FREQUENCY = 48000;

	private String modelPath;

	/**
	 * Instantiates a new FTWTTS engine.
	 * 
	 * @param context
	 *            the context
	 */
	public FTWTTSEngine(Context context) {
		Log.i("FTWTTSEngine", "Copying assets.");
		copyAssets(context);
	}


	public synchronized void synthesize(String txt) {

		short[] cache = nativeSynthesize(txt, "1.0", modelPath);

		AudioTrack audioTrack = new AudioTrack(
				AudioManager.STREAM_MUSIC,
				MODEL_FREQUENCY,
				AudioFormat.CHANNEL_OUT_MONO,
				AudioFormat.ENCODING_PCM_16BIT,
				currentElement.getSampleCache().length * 2,
				AudioTrack.MODE_STATIC);

		audioTrack.write(currentElement.getSampleCache(), 0,
				currentElement.getSampleCache().length);
		audioTrack.play();		
	}

	/**
	 * Copies assets needed for speech synthesis to the data folder.
	 * 
	 * @param context
	 *            the context used
	 */
	private synchronized void copyAssets(Context context) {
		modelPath = context.getFilesDir().getAbsolutePath() + "/";
		copyFile(context, "leo.htsvoice.jpg", modelPath);
		copyFile(context, "leo.rules.jpg", modelPath);
	}

	/**
	 * Copy file.
	 * 
	 * @param context
	 *            the context
	 * @param filename
	 *            the filename
	 */
	private synchronized void copyFile(Context context, String filename,
			String modelPath) {
		AssetManager assetManager = context.getAssets();

		InputStream in = null;
		OutputStream out = null;
		String newFileName = null;
		try {
			in = assetManager.open(filename);
			if (filename.endsWith(".jpg")) // extension was added to avoid
											// compression on APK file
				newFileName = modelPath
						+ filename.substring(0, filename.length() - 4);
			else
				newFileName = modelPath + filename;

			out = new FileOutputStream(newFileName);

			byte[] buffer = new byte[1024];
			int read;
			while ((read = in.read(buffer)) != -1) {
				out.write(buffer, 0, read);
			}
			in.close();
			in = null;
			out.flush();
			out.close();
			out = null;
		} catch (Exception e) {
			Log.e("ftw", "Exception in copyFile() " + e.toString());
		}
	}


	/**
	 * NDK call to native speech synthesis library.
	 * 
	 * @param utterance
	 *            utterance to speak.
	 * @return the short[]
	 */
	private native short[] nativeSynthesize(String utterance, String rate,
			String path);

	static {
		System.loadLibrary("SALB");
	}
}

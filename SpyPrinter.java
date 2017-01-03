package com.yahoo.mobile.client.android.weather.samsung;
import android.os.Debug;
import android.util.Log;
import android.util.Printer;

public class SpyPrinter implements Printer {

	private static final int MEMORY_INFO_FREQ = 5000; //dump memory use every 5 seconds
	private long lastMemoryStat = 0;

	@Override
	public void println(String looperMessage) {
		long	now;


		Log.d("mainlooper ", looperMessage);

		now = System.currentTimeMillis();
		if ((lastMemoryStat  - now) > MEMORY_INFO_FREQ) {
			lastMemoryStat = now;
			Debug.MemoryInfo memoryInfo = new Debug.MemoryInfo();
			Debug.getMemoryInfo(memoryInfo);

			String memMessage = String.format("Pss=%.2f MB\nPrivate=%.2f MB\nShared=%.2f MB",
					memoryInfo.getTotalPss() / 1024.0,
					memoryInfo.getTotalPrivateDirty() / 1024.0,
					memoryInfo.getTotalSharedDirty() / 1024.0);

			Log.i("memorystat", memMessage);
			
			// alert if over memory use and make it very annoying.
			if (memoryInfo.getTotalPrivateDirty() > 20*1024*1024) {
				for (int i = 0; i < 20; i++) {
					Log.i("memorystat ", "over the wall");
				}
			}
		}
	}

}

package com.shawn.activity;

import com.shawn.activity.R;  

import android.app.Activity;  
import android.content.Intent;  
import android.os.Bundle;  
import android.util.Log;  
import android.view.View;  
import android.view.View.OnClickListener;  
import android.widget.Button;  

public class MainActivity extends Activity  implements OnClickListener {
	private final static String LOG_TAG = "com.shawn.activity.MainActivity";  

	private Button startButton = null;  

	@Override  
	public void onCreate(Bundle savedInstanceState) {  
		super.onCreate(savedInstanceState);  
		setContentView(R.layout.main);  

		startButton = (Button)findViewById(R.id.button_start);  
		startButton.setOnClickListener(this);  

		Log.i(LOG_TAG, "Main Activity Created.");  
	}  

	@Override  
	public void onClick(View v) {  
		if(v.equals(startButton)) {  
			Intent intent = new Intent("com.shawn.activity.subactivity");  
			startActivity(intent);  
		}  
	}  
}

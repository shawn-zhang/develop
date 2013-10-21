package com.example.androidgps;

import android.location.Criteria;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.provider.Settings;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.view.Menu;
import android.widget.TextView;
import android.widget.Toast;

@SuppressLint("ShowToast") public class MainActivity extends Activity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        opengpssetting();
        getLocation();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.activity_main, menu);
        return true;
    }
    
    private void opengpssetting(){
    	LocationManager lm = (LocationManager)this.getSystemService(LOCATION_SERVICE);
    	
    	if (lm.isProviderEnabled(android.location.LocationManager.GPS_PROVIDER)){
    		Toast.makeText(this, "GPS模块开启", Toast.LENGTH_SHORT);
    		return;
    	}
    	
    	Toast.makeText(this, "GPS模块未开启！", Toast.LENGTH_SHORT);
    	Intent intent = new Intent(Settings.ACTION_SECURITY_SETTINGS);
    	startActivityForResult(intent, 0);
    }
    private void getLocation(){
    	LocationManager lm = (LocationManager)this.getSystemService(LOCATION_SERVICE);
    	
    	Criteria criteria = new Criteria();
    	criteria.setAccuracy(Criteria.ACCURACY_COARSE);//设置为最大精度
    	criteria.setAltitudeRequired(false);//不要求海拔信息
    	criteria.setBearingRequired(false);//不要求方位信息
    	criteria.setCostAllowed(true);//是否允许付费
    	criteria.setPowerRequirement(Criteria.POWER_HIGH);//对电量的要求
    	
    	String provider = lm.getBestProvider(criteria, true); // 获取GPS信息
        Location location = lm.getLastKnownLocation(provider); // 通过GPS获取位置
        updateToNewLocation(location);
        
        // 设置监听器，自动更新的最小时间为间隔N秒(1秒为1*1000，这样写主要为了方便)或最小位移变化超过N米
        lm.requestLocationUpdates(provider, 100 * 1000, 500,locationListener);
    }

    private void updateToNewLocation(Location location) {

            TextView tv1;
            tv1 = (TextView) this.findViewById(R.id.tv1);
            if (location != null) {
                double  latitude = location.getLatitude();
                double longitude= location.getLongitude();
                tv1.setText("维度：" +  latitude+ "\n经度" + longitude);
            } else {
                tv1.setText("无法获取地理信息");
            }

    }
    private final LocationListener locationListener = new LocationListener() {

		public void onLocationChanged(Location location) {
			// TODO Auto-generated method stub
			updateToNewLocation(location);
		}

		public void onProviderDisabled(String arg0) {
			// TODO Auto-generated method stub
			
		}

		public void onProviderEnabled(String arg0) {
			// TODO Auto-generated method stub
			
		}

		public void onStatusChanged(String arg0, int arg1, Bundle arg2) {
			// TODO Auto-generated method stub
			
		}
    	
    };
    
}

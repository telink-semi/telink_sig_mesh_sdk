/********************************************************************************************************
 * @file SceneSettingActivity.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *
 *******************************************************************************************************/
package com.telink.sig.mesh.demo.ui;

import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Switch;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.SharedPreferenceHelper;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.model.Mesh;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.util.Arrays;

/**
 * special debug func
 * Created by kee on 2018/9/18.
 */
public class DebugActivity extends BaseActivity implements View.OnClickListener {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_debug);
        setTitle("Mesh Debug");

        enableBackNav(true);
        findViewById(R.id.btn_iv).setOnClickListener(this);
        findViewById(R.id.btn_sno).setOnClickListener(this);

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_iv:
                MeshService.getInstance().ivTest();
                break;

            case R.id.btn_sno:
                MeshService.getInstance().snoTest();
                break;

        }
    }
}
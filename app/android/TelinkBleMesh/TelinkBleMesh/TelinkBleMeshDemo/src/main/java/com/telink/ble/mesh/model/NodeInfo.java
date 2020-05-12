package com.telink.ble.mesh.model;

import android.os.Handler;
import android.util.SparseBooleanArray;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.entity.Scheduler;
import com.telink.ble.mesh.util.MeshLogger;


import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by kee on 2019/8/22.
 */

public class NodeInfo implements Serializable {

    /**
     * networking state
     */
    public static final int STATE_PROVISION_FAIL = -1;

    public static final int STATE_PROVISIONING = 0;

    // binding equals provision success
    public static final int STATE_BINDING = 1;

    public static final int STATE_BIND_SUCCESS = 2;

    public static final int STATE_BIND_FAIL = -2;

    public int state;

    /**
     * on/off state
     */
    public static final int ON_OFF_STATE_ON = 1;

    public static final int ON_OFF_STATE_OFF = 0;

    public static final int ON_OFF_STATE_OFFLINE = -1;


    /**
     * state description
     */
    public String stateDesc;

    /**
     * primary element unicast address
     */
    public int meshAddress;

    /**
     * mac address
     */
    public String macAddress;
    /**
     * device-uuid from scan-record when normal provision
     * or
     * device scan report when fast-provision or remote-provision
     */
    public byte[] deviceUUID;

    /**
     * element count
     */
    public int elementCnt = 0;

    public byte[] deviceKey;

    /**
     * device subscription/group info
     */
    public List<Integer> subList = new ArrayList<>();

    // device lightness
    public int lum = 0;

    // device temperature
    public int temp = 0;

    /**
     * device on off state
     * 0:off 1:on -1:offline
     */
    private int onOff = ON_OFF_STATE_OFFLINE;

    /**
     * composition data
     * {@link com.telink.ble.mesh.core.message.config.CompositionDataStatusMessage}
     */
    public CompositionData compositionData = null;


    // is relay enabled
    private boolean relayEnable = true;

    /**
     * scheduler
     */
    public List<Scheduler> schedulers = new ArrayList<>();

    /**
     * publication
     */
    private PublishModel publishModel;

    /**
     * default bind support
     */
    private boolean defaultBind = false;

    /**
     * selected for UI select
     */
    public boolean selected = false;

    private OfflineCheckTask offlineCheckTask = new OfflineCheckTask() {
        @Override
        public void run() {
            onOff = -1;
            MeshLogger.log("offline check task running");
            TelinkMeshApplication.getInstance().dispatchEvent(new NodeStatusChangedEvent(TelinkMeshApplication.getInstance(), NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, NodeInfo.this));
        }
    };

    public int getOnOff() {
        return onOff;
    }

    public void setOnOff(int onOff) {
        this.onOff = onOff;
        if (publishModel != null) {
            Handler handler = TelinkMeshApplication.getInstance().getOfflineCheckHandler();
            handler.removeCallbacks(offlineCheckTask);
            int timeout = publishModel.period * 3 + 2;
            if (this.onOff != -1 && timeout > 0) {
                handler.postDelayed(offlineCheckTask, timeout);
            }
        }
    }


    public boolean isPubSet() {
        return publishModel != null;
    }

    public PublishModel getPublishModel() {
        return publishModel;
    }

    public void setPublishModel(PublishModel model) {
        this.publishModel = model;

        Handler handler = TelinkMeshApplication.getInstance().getOfflineCheckHandler();
        handler.removeCallbacks(offlineCheckTask);
        if (this.publishModel != null && this.onOff != -1) {
            int timeout = publishModel.period * 3 + 2;
            if (timeout > 0) {
                handler.postDelayed(offlineCheckTask, timeout);
            }
        }
    }

    public boolean isRelayEnable() {
        return relayEnable;
    }

    public void setRelayEnable(boolean relayEnable) {
        this.relayEnable = relayEnable;
    }

    public Scheduler getSchedulerByIndex(byte index) {
        if (schedulers == null || schedulers.size() == 0) {
            return null;
        }
        for (Scheduler scheduler : schedulers) {
            if (scheduler.getIndex() == index) {
                return scheduler;
            }
        }
        return null;
    }

    public void saveScheduler(Scheduler scheduler) {
        if (schedulers == null) {
            schedulers = new ArrayList<>();
            schedulers.add(scheduler);
        } else {
            for (int i = 0; i < schedulers.size(); i++) {
                if (schedulers.get(i).getIndex() == scheduler.getIndex()) {
                    schedulers.set(i, scheduler);
                    return;
                }
            }
            schedulers.add(scheduler);
        }

    }

    // 0 - 15/0x0f
    public byte allocSchedulerIndex() {
        if (schedulers == null || schedulers.size() == 0) {
            return 0;
        }

        outer:
        for (byte i = 0; i <= 0x0f; i++) {
            for (Scheduler scheduler : schedulers) {
                if (scheduler.getIndex() == i) {
                    continue outer;
                }
            }
            return i;
        }
        return -1;
    }

    public String getOnOffDesc() {
        if (this.onOff == 1) {
            return "ON";
        } else if (this.onOff == 0) {
            return "OFF";
        } else if (this.onOff == -1) {
            return "OFFLINE";
        }
        return "UNKNOWN";
    }

    /**
     * get on/off model element info
     * in panel , multi on/off may exist in different element
     *
     * @return adr
     */
    public List<Integer> getOnOffEleAdrList() {
        if (compositionData == null) return null;
        List<Integer> addressList = new ArrayList<>();

        // element address is based on primary address and increase in loop
        int eleAdr = this.meshAddress;
        outer:
        for (CompositionData.Element element : compositionData.elements) {
            if (element.sigModels != null) {
                for (int modelId : element.sigModels) {
                    if (modelId == MeshSigModel.SIG_MD_G_ONOFF_S.modelId) {
                        addressList.add(eleAdr++);
                        continue outer;
                    }
                }
            }
            eleAdr++;
        }

        return addressList;
    }

    /**
     * @param tarModelId target model id
     * @return element address: -1 err
     */
    public int getTargetEleAdr(int tarModelId) {
        if (compositionData == null) return -1;
        int eleAdr = this.meshAddress;
        for (CompositionData.Element element : compositionData.elements) {
            if (element.sigModels != null) {
                for (int modelId : element.sigModels) {
                    if (modelId == tarModelId) {
                        return eleAdr;
                    }
                }
            }

            if (element.vendorModels != null) {
                for (int modelId : element.vendorModels) {
                    if (modelId == tarModelId) {
                        return eleAdr;
                    }
                }
            }

            eleAdr++;
        }
        return -1;
    }

    /**
     * get lum model element
     *
     * @return lum lightness union info
     */
    public SparseBooleanArray getLumEleInfo() {
        if (compositionData == null) return null;
        int eleAdr = this.meshAddress;

        SparseBooleanArray result = new SparseBooleanArray();

        for (CompositionData.Element element : compositionData.elements) {
            if (element.sigModels != null) {
                boolean levelSupport = false;
                boolean lumSupport = false;
                // if contains lightness model
                for (int modelId : element.sigModels) {
                    if (modelId == MeshSigModel.SIG_MD_LIGHTNESS_S.modelId) {
                        lumSupport = true;
                    }
                    if (modelId == MeshSigModel.SIG_MD_G_LEVEL_S.modelId) {
                        levelSupport = true;
                    }
                }

                if (lumSupport) {
                    result.append(eleAdr, levelSupport);
                    return result;
                }
            }
            eleAdr++;
        }
        return null;
    }

    /**
     * get element with temperature model
     *
     * @return temp & isLevelSupported
     */
    public SparseBooleanArray getTempEleInfo() {
        if (compositionData == null) return null;
        int eleAdr = this.meshAddress;

        SparseBooleanArray result = new SparseBooleanArray();

        for (CompositionData.Element element : compositionData.elements) {
            if (element.sigModels != null) {
                boolean levelSupport = false;
                boolean tempSupport = false;
                // contains temperature model
                for (int modelId : element.sigModels) {
                    if (modelId == MeshSigModel.SIG_MD_LIGHT_CTL_TEMP_S.modelId) {
                        tempSupport = true;
                    }
                    if (modelId == MeshSigModel.SIG_MD_G_LEVEL_S.modelId) {
                        levelSupport = true;
                    }
                }

                if (tempSupport) {
                    result.append(eleAdr, levelSupport);
                    return result;
                }
            }
            eleAdr++;
        }
        return null;
    }


    public String getStateDesc() {
        switch (state) {
            case STATE_PROVISION_FAIL:
                return "Provision Fail -- " + stateDesc;

            case STATE_PROVISIONING:
                return "Provisioning";

            case STATE_BINDING:
                return "Key Binding" + (defaultBind ? "(default)" : "");

            case STATE_BIND_SUCCESS:
                return "Key Bind Success" + (defaultBind ? "(default)" : "");

            case STATE_BIND_FAIL:
                return "Key Bind Fail -- " + stateDesc;
        }
        return "UNKNOWN";
    }

    public String getPidDesc() {
        String pidInfo = "";
        if (state == STATE_BIND_SUCCESS) {
            pidInfo = (compositionData.cid == 0x0211 ? String.format("%02X", compositionData.pid)
                    : "cid-" + String.format("%02X", compositionData.cid));

        } else {
            pidInfo = "(unbound)";
        }
        return pidInfo;
    }

    public boolean isDefaultBind() {
        return defaultBind;
    }

    public void setDefaultBind(boolean defaultBind) {
        this.defaultBind = defaultBind;
    }

    public boolean isLpn() {
        return this.compositionData != null && this.compositionData.lowPowerSupport();
    }
}
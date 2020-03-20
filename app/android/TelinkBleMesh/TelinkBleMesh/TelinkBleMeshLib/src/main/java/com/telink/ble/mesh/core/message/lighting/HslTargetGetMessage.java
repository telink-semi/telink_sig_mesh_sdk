package com.telink.ble.mesh.core.message.lighting;

import com.telink.ble.mesh.core.message.Opcode;

/**
 * Created by kee on 2019/9/19.
 */

public class HslTargetGetMessage extends LightingMessage {

    public static HslTargetGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        HslTargetGetMessage message = new HslTargetGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    public HslTargetGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.LIGHT_HSL_TARGET_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.LIGHT_HSL_TARGET_STATUS.value;
    }
}

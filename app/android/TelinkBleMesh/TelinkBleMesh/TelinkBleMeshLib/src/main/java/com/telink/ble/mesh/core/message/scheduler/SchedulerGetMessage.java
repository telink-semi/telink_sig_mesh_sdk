/********************************************************************************************************
 * @file SchedulerGetMessage.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date     Sep. 30, 2017
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/
package com.telink.ble.mesh.core.message.scheduler;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.lighting.LightingMessage;


/**
 * This class represents a SchedulerGetMessage, which is a type of LightingMessage.
 * It is used to retrieve information about a scheduler.
 */
public class SchedulerGetMessage extends LightingMessage {
    /**
     * Creates a new SchedulerGetMessage with the specified destinationAddress and appKeyIndex.
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex The app key index of the message.
     */
    public SchedulerGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Creates a new SchedulerGetMessage with the specified destinationAddress, appKeyIndex, and rspMax.
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex The app key index of the message.
     * @param rspMax The maximum number of responses to retrieve.
     * @return The created SchedulerGetMessage.
     */
    public static SchedulerGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        SchedulerGetMessage message = new SchedulerGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    /**
     * Returns the opcode value for a SchedulerGetMessage.
     * @return The opcode value for a SchedulerGetMessage.
     */
    @Override
    public int getOpcode() {
        return Opcode.SCHD_GET.value;
    }

    /**
     * Returns the opcode value for the response message to a SchedulerGetMessage.
     * @return The opcode value for the response message to a SchedulerGetMessage.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.SCHD_STATUS.value;
    }
}
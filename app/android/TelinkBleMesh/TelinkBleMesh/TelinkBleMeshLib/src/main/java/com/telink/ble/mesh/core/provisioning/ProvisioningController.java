package com.telink.ble.mesh.core.provisioning;

import android.os.Handler;
import android.os.HandlerThread;

import com.telink.ble.mesh.core.Encipher;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningCapabilityPDU;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningConfirmPDU;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningDataPDU;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningInvitePDU;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningPDU;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningPubKeyPDU;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningRandomPDU;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningStartPDU;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningStatePDU;
import com.telink.ble.mesh.core.proxy.ProxyPDU;
import com.telink.ble.mesh.entity.ProvisioningDevice;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import org.spongycastle.jcajce.provider.asymmetric.ec.BCECPublicKey;

import java.nio.ByteBuffer;
import java.security.KeyPair;

import androidx.annotation.NonNull;

/**
 * provisioning
 * OOB public key is not supported
 * Auth Method inputOOB outputOOB is not supported
 * Created by kee on 2019/7/31.
 */

public class ProvisioningController {
    private final String LOG_TAG = "Provisioning";

    /**
     * provisioning state
     * indicates current state in provisioning flow
     * init state {@link #STATE_IDLE} means not in provisioning flow
     * direction introduction
     * P: provisioner, D: device
     * invite(P->D)
     * =>
     * capability(D->P)
     * =>
     * start(P->D)
     * =>
     * pub_key(P->D)
     * =>
     * pub_key(D->P)
     * =>
     * confirm(P->D)
     * =>
     * confirm(D->P)
     * =>
     * random(P->D)
     * =>
     * random(D->P)
     * =>
     * check confirm
     * =>
     * provisioning end
     */
    private int state = STATE_IDLE;

    /**
     * not in provisioning flow
     */
    public static final int STATE_IDLE = 0x1000;

    /**
     * sent provisioning invite pdu
     */
    public static final int STATE_INVITE = 0x1001;

    /**
     * received provisioning capability
     */
    public static final int STATE_CAPABILITY = 0x1002;

    /**
     * sent provisioning start
     */
    public static final int STATE_START = 0x1003;

    /**
     * sent provisioning pub key
     */
    public static final int STATE_PUB_KEY_SENT = 0x1004;

    /**
     * received provisioning pub key
     */
    public static final int STATE_PUB_KEY_RECEIVED = 0x1005;


//    public static final int STATE_INPUT_COMPLETE = 0x1005;

    /**
     * sent provisioning confirm
     */
    public static final int STATE_CONFIRM_SENT = 0x1006;

    /**
     * received provisioning confirm
     */
    public static final int STATE_CONFIRM_RECEIVED = 0x1007;

    /**
     * sent provisioning random
     */
    public static final int STATE_RANDOM_SENT = 0x1008;

    /**
     * received provisioning random
     */
    public static final int STATE_RANDOM_RECEIVED = 0x1009;

    /**
     * sent provisioning data
     */
    public static final int STATE_DATA = 0x100A;

    /**
     * received provisioning complete, success!
     */
    public static final int STATE_COMPLETE = 0x100B;

    /**
     * received provisioning fail, or params check err!
     */
    public static final int STATE_FAILED = 0x100C;


    private static final byte[] AUTH_NO_OOB = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    private static final long TIMEOUT_PROVISIONING = 60 * 1000;

    private Handler delayHandler;

    private ProvisioningBridge mProvisioningBridge;

    private ProvisioningInvitePDU invitePDU;

    private ProvisioningStartPDU startPDU;

    private ProvisioningCapabilityPDU pvCapability;

    private ProvisioningPubKeyPDU provisionerPubKeyPDU;

    private ProvisioningPubKeyPDU devicePubKeyPDU;

    private KeyPair provisionerKeyPair;

    private byte[] deviceECDHSecret;

    private byte[] provisionerRandom;

    private byte[] deviceRandom;

    private byte[] deviceConfirm;

//    private byte[] deviceKey;


//    private ProvisioningParams mProvisioningParams;

    /**
     * target device
     */
    private ProvisioningDevice mProvisioningDevice;

    public ProvisioningController(HandlerThread handlerThread) {
        this.delayHandler = new Handler(handlerThread.getLooper());
    }

    public void setProvisioningBridge(ProvisioningBridge provisioningBridge) {
        this.mProvisioningBridge = provisioningBridge;
    }

    public ProvisioningDevice getProvisioningDevice() {
        return mProvisioningDevice;
    }

    public void begin(@NonNull ProvisioningDevice device) {
        log("begin -- " + Arrays.bytesToHexString(device.getDeviceUUID()));
        this.mProvisioningDevice = device;

        delayHandler.removeCallbacks(provisioningTimeoutTask);
        delayHandler.postDelayed(provisioningTimeoutTask, TIMEOUT_PROVISIONING);
        provisionInvite();
    }

    public void clear() {
        if (delayHandler != null) {
            delayHandler.removeCallbacks(provisioningTimeoutTask);
        }
        this.state = STATE_IDLE;
    }


    /*public void begin(byte[] networkKey, int netKeyIndex, int ivIndex, int unicast, byte[] authValue) {
        this.mProvisioningParams = ProvisioningParams.getDefault(networkKey, netKeyIndex, ivIndex, unicast);
        this.authValue = authValue;
        provisionInvite();
    }*/

    public void pushNotification(byte[] provisioningPdu) {
        if (state == STATE_IDLE) {
            log("received notification when idle", MeshLogger.LEVEL_WARN);
            return;
        }
        log("provisioning pdu received: " + Arrays.bytesToHexString(provisioningPdu, ""));
        int provisioningPduType = provisioningPdu[0];
        byte[] provisioningData = new byte[provisioningPdu.length - 1];
        System.arraycopy(provisioningPdu, 1, provisioningData, 0, provisioningData.length);
        switch (provisioningPduType) {
            case ProvisioningPDU.TYPE_CAPABILITIES:
                onCapabilityReceived(provisioningData);
                break;

            case ProvisioningPDU.TYPE_PUBLIC_KEY:

                onPubKeyReceived(provisioningData);
                break;

            case ProvisioningPDU.TYPE_CONFIRMATION:
                onConfirmReceived(provisioningData);
                break;

            case ProvisioningPDU.TYPE_RANDOM:
                onRandomReceived(provisioningData);
                break;

            case ProvisioningPDU.TYPE_COMPLETE:
                onProvisionSuccess();
                break;
            case ProvisioningPDU.TYPE_FAILED:
                onProvisionFail("failed notification received");
                break;
        }
    }

    private void onProvisionSuccess() {
        updateProvisioningState(STATE_COMPLETE, "Provision Success");
        onProvisionComplete();
    }

    private void onProvisionComplete() {
        this.state = STATE_IDLE;
        delayHandler.removeCallbacks(provisioningTimeoutTask);
    }

    private byte[] getAuthValue() {
        if (pvCapability.staticOOBSupported()) {
            return mProvisioningDevice.getAuthValue();
        } else {
            return AUTH_NO_OOB;
        }
    }

    private void updateProvisioningState(int state, String desc) {
        log("provisioning state update: state -- " + state + " desc -- " + desc);
        this.state = state;
        if (mProvisioningBridge != null) {
            mProvisioningBridge.onProvisionStateChanged(state, desc);
        }
    }

    /**
     * invite
     */
    private void provisionInvite() {
        byte attention = 0;
        invitePDU = new ProvisioningInvitePDU(attention);
        updateProvisioningState(STATE_INVITE, "Invite");
        sendProvisionPDU(invitePDU);
    }


    private void provisionStart(boolean isStaticOOB) {
        startPDU = ProvisioningStartPDU.getSimple(isStaticOOB);
        updateProvisioningState(STATE_START, "Start - use static oob?" + isStaticOOB);
        sendProvisionPDU(startPDU);
    }

    private void provisionSendPubKey() {
        ProvisioningPubKeyPDU pubKeyPDU = getPublicKey();
        updateProvisioningState(STATE_PUB_KEY_SENT, "Send Public Key");
        sendProvisionPDU(pubKeyPDU);
    }

    public void onCapabilityReceived(byte[] capData) {
        if (this.state != STATE_INVITE) {
            log(" capability received when not inviting", MeshLogger.LEVEL_WARN);
            return;
        }
        updateProvisioningState(STATE_CAPABILITY, "Capability Received");
        pvCapability = ProvisioningCapabilityPDU.fromBytes(capData);
        mProvisioningDevice.setDeviceCapability(pvCapability);
        boolean useStaticOOB = pvCapability.staticOOBSupported();
        if (useStaticOOB && mProvisioningDevice.getAuthValue() == null) {
            if (mProvisioningDevice.isAutoUseNoOOB()) {
                // use no oob
                useStaticOOB = false;
            } else {
                onProvisionFail("authValue not found when device static oob supported!");
                return;
            }
        }
        provisionStart(useStaticOOB);
        provisionSendPubKey();
    }

    private void onPubKeyReceived(byte[] pubKeyData) {
        if (this.state != STATE_PUB_KEY_SENT) {
            log(" pub key received when not pub key sent", MeshLogger.LEVEL_WARN);
            return;
        }

        updateProvisioningState(STATE_PUB_KEY_RECEIVED, "Public Key received");
        log("pub key received: " + Arrays.bytesToHexString(pubKeyData, ":"));
        devicePubKeyPDU = ProvisioningPubKeyPDU.fromBytes(pubKeyData);
        deviceECDHSecret = Encipher.generateECDH(pubKeyData, provisionerKeyPair.getPrivate());
        log("get secret: " + Arrays.bytesToHexString(deviceECDHSecret, ":"));
        sendConfirm();
    }


    private void sendConfirm() {
        ProvisioningConfirmPDU confirmPDU = new ProvisioningConfirmPDU(getConfirm());
        updateProvisioningState(STATE_CONFIRM_SENT, "Send confirm");
        sendProvisionPDU(confirmPDU);
    }


    private byte[] getConfirm() {
        byte[] confirmInput = confirmAssembly();
        byte[] salt = Encipher.generateSalt(confirmInput);
        byte[] confirmationKey = Encipher.k1(deviceECDHSecret, salt, Encipher.PRCK);

        provisionerRandom = Arrays.generateRandom(16);
        byte[] authValue = getAuthValue();


        byte[] confirmData = new byte[provisionerRandom.length + authValue.length];
        System.arraycopy(provisionerRandom, 0, confirmData, 0, provisionerRandom.length);
        System.arraycopy(authValue, 0, confirmData, provisionerRandom.length, authValue.length);

        return Encipher.aesCmac(confirmData, confirmationKey);
    }

    private void onConfirmReceived(byte[] confirm) {
        if (this.state != STATE_CONFIRM_SENT) {
            log(" confirm received when not confirm sent", MeshLogger.LEVEL_WARN);
            return;
        }

        updateProvisioningState(STATE_CONFIRM_RECEIVED, "Confirm received");
        deviceConfirm = confirm;
        sendRandom();
    }


    private void onRandomReceived(byte[] random) {
        if (this.state != STATE_RANDOM_SENT) {
            log(" random received when not random sent", MeshLogger.LEVEL_WARN);
            return;
        }

        updateProvisioningState(STATE_RANDOM_RECEIVED, "Random received");
        deviceRandom = random;
        boolean pass = checkDeviceConfirm(random);
        if (pass) {
            sendProvisionData();
        } else {
            onProvisionFail("device confirm check err!");
        }
    }

    private void onProvisionFail(String desc) {
        updateProvisioningState(STATE_FAILED, desc);
        onProvisionComplete();
    }

    private void sendProvisionData() {
        byte[] pvData = createProvisioningData();
        ProvisioningDataPDU provisioningDataPDU = new ProvisioningDataPDU(pvData);
        updateProvisioningState(STATE_DATA, "Send provisioning data");
        sendProvisionPDU(provisioningDataPDU);
    }

    private void sendRandom() {
        ProvisioningRandomPDU randomPDU = new ProvisioningRandomPDU(provisionerRandom);
        updateProvisioningState(STATE_RANDOM_SENT, "Send random");
        sendProvisionPDU(randomPDU);
    }


    private ProvisioningPubKeyPDU getPublicKey() {

        provisionerKeyPair = Encipher.generateKeyPair();
        if (provisionerKeyPair != null) {
            BCECPublicKey publicKey = (BCECPublicKey) provisionerKeyPair.getPublic();
            byte[] x = publicKey.getQ().getXCoord().getEncoded();
            byte[] y = publicKey.getQ().getYCoord().getEncoded();
            provisionerPubKeyPDU = new ProvisioningPubKeyPDU();
            provisionerPubKeyPDU.x = x;
            provisionerPubKeyPDU.y = y;
            log("get key x: " + Arrays.bytesToHexString(x, ":"));
            log("get key y: " + Arrays.bytesToHexString(y, ":"));
            return provisionerPubKeyPDU;
        } else {
            throw new RuntimeException("key pair generate err");
        }
    }

    // assemble random value
    public byte[] confirmAssembly() {
        byte[] inviteData = invitePDU.toBytes();
        byte[] capData = pvCapability.toBytes();
        byte[] startData = startPDU.toBytes();

        byte[] provisionerPubKey = provisionerPubKeyPDU.toBytes();

        byte[] devicePubKey = devicePubKeyPDU.toBytes();

        final int len = inviteData.length
                + capData.length
                + startData.length
                + provisionerPubKey.length
                + devicePubKey.length;
        ByteBuffer buffer = ByteBuffer.allocate(len);
        buffer.put(inviteData)
                .put(capData)
                .put(startData)
                .put(provisionerPubKey)
                .put(devicePubKey);

        return buffer.array();
    }

    private boolean checkDeviceConfirm(byte[] random) {

        byte[] confirmationInputs = confirmAssembly();
        byte[] confirmationSalt = Encipher.generateSalt(confirmationInputs);

        byte[] confirmationKey = Encipher.k1(deviceECDHSecret, confirmationSalt, Encipher.PRCK);
        byte[] authenticationValue = getAuthValue();

        ByteBuffer buffer = ByteBuffer.allocate(random.length + authenticationValue.length);
        buffer.put(random);
        buffer.put(authenticationValue);
        final byte[] confirmationData = buffer.array();

        final byte[] confirmationValue = Encipher.aesCmac(confirmationData, confirmationKey);

        if (java.util.Arrays.equals(confirmationValue, deviceConfirm)) {
            log("Confirmation values check pass");
            return true;
        } else {
            log("Confirmation values check err", MeshLogger.LEVEL_WARN);
        }

        return false;
    }


    private byte[] createProvisioningData() {

        byte[] confirmationInputs = confirmAssembly();

        byte[] confirmationSalt = Encipher.generateSalt(confirmationInputs);

        ByteBuffer saltBuffer = ByteBuffer.allocate(confirmationSalt.length + provisionerRandom.length + deviceRandom.length);
        saltBuffer.put(confirmationSalt);
        saltBuffer.put(provisionerRandom);
        saltBuffer.put(deviceRandom);
        byte[] provisioningSalt = Encipher.generateSalt(saltBuffer.array());

        byte[] t = Encipher.aesCmac(deviceECDHSecret, provisioningSalt);
        byte[] sessionKey = Encipher.aesCmac(Encipher.PRSK, t);

        byte[] nonce = Encipher.k1(deviceECDHSecret, provisioningSalt, Encipher.PRSN);
        ByteBuffer nonceBuffer = ByteBuffer.allocate(nonce.length - 3);
        nonceBuffer.put(nonce, 3, nonceBuffer.limit());
        byte[] sessionNonce = nonceBuffer.array();

        mProvisioningDevice.setDeviceKey(Encipher.aesCmac(Encipher.PRDK, t));
        log("device key: " + Arrays.bytesToHexString(mProvisioningDevice.getDeviceKey(), ":"));
        log("provisioning data prepare: " + mProvisioningDevice.toString());
        byte[] provisioningData = mProvisioningDevice.generateProvisioningData();

        log("unencrypted provision data: " + Arrays.bytesToHexString(provisioningData, ":"));
        byte[] enData = Encipher.ccm(provisioningData, sessionKey, sessionNonce, 8, true);
        log("encrypted provision data: " + Arrays.bytesToHexString(enData, ":"));
        return enData;
    }

    private void sendProvisionPDU(ProvisioningStatePDU pdu) {
        byte[] data = pdu.toBytes();

        byte[] re;
        if (data == null || data.length == 0) {
            re = new byte[]{pdu.getState()};
        } else {
            re = new byte[data.length + 1];
            re[0] = pdu.getState();
            System.arraycopy(data, 0, re, 1, data.length);
        }
        if (mProvisioningBridge != null) {
            log("pdu prepared: " + Arrays.bytesToHexString(data, ":"));
            mProvisioningBridge.onCommandPrepared(ProxyPDU.TYPE_PROVISIONING_PDU, re);
        }
    }

    private Runnable provisioningTimeoutTask = new Runnable() {
        @Override
        public void run() {
            onProvisionFail("provisioning timeout");
        }
    };

    private void log(String logMessage) {
        log(logMessage, MeshLogger.LEVEL_DEBUG);
    }

    private void log(String logMessage, int level) {
        MeshLogger.log(logMessage, LOG_TAG, level);
    }
}

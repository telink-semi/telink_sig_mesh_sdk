package com.telink.ble.mesh.ui.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.ui.IconGenerator;

import java.util.List;

import androidx.recyclerview.widget.RecyclerView;

/**
 * 设备列表适配器
 * Created by Administrator on 2016/10/25.
 */
public class OnlineDeviceListAdapter extends BaseRecyclerViewAdapter<OnlineDeviceListAdapter.ViewHolder> {
    List<NodeInfo> mDevices;
    Context mContext;

    public OnlineDeviceListAdapter(Context context, List<NodeInfo> devices) {
        mContext = context;
        mDevices = devices;
    }

    public void resetDevices(List<NodeInfo> devices) {
        this.mDevices = devices;
        notifyDataSetChanged();
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_online_device, null, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_name = itemView.findViewById(R.id.tv_name);
        holder.img_icon = itemView.findViewById(R.id.img_icon);
        return holder;
    }

    @Override
    public int getItemCount() {
        return mDevices == null ? 0 : mDevices.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);

        NodeInfo device = mDevices.get(position);
        final int deviceType = device.compositionData != null && device.compositionData.lowPowerSupport() ? 1 : 0;
        holder.img_icon.setImageResource(IconGenerator.getIcon(deviceType, device.getOnOff()));

        if (device.meshAddress == MeshService.getInstance().getDirectConnectedNodeAddress()) {
            holder.tv_name.setTextColor(mContext.getResources().getColor(R.color.colorPrimary));
        }else {
            holder.tv_name.setTextColor(mContext.getResources().getColor(R.color.black));
        }

        /*if (device.getOnOff() == -1) {
//            holder.img_icon.setImageResource(R.drawable.icon_light_offline);
            holder.tv_name.setTextColor(mContext.getResources().getColor(R.color.black));
        } else {
            if (device.macAddress != null && device.macAddress.equals(MeshService.getInstance().getCurDeviceMac())) {
                holder.tv_name.setTextColor(mContext.getResources().getColor(R.color.colorPrimary));
            } else {
                holder.tv_name.setTextColor(mContext.getResources().getColor(R.color.black));
            }
        }*/


//        holder.tv_name.setText(models.get(position).getAddress());
        String info;
        if (device.meshAddress <= 0xFF) {
            info = String.format("%02X", device.meshAddress);
        } else {
            info = String.format("%04X", device.meshAddress);
        }


        if (device.state == NodeInfo.STATE_BIND_SUCCESS) {


            info += (device.compositionData.cid == 0x0211 ? "(Pid-" + String.format("%02X", device.compositionData.pid) + ")"
                    : "(cid-" + String.format("%02X", device.compositionData.cid) + ")");


            /*if (device.nodeInfo.cpsData.lowPowerSupport()) {
                info += "LPN";
            }*/
            /*" : " +(device.getOnOff() == 1 ? Math.max(0, device.lum) : 0) + " : " +
                    device.temp*/
        } else {
            info += "(unbound)";
        }
        holder.tv_name.setText(info);
    }

    class ViewHolder extends RecyclerView.ViewHolder {


        public ImageView img_icon;
        public TextView tv_name;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}

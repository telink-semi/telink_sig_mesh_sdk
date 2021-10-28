/********************************************************************************************************
 * @file     CustomAlertView.h
 *
 * @brief    A concise description.
 *
 * @author       梁家誌
 * @date         2021/9/16
 *
 * @par      Copyright © 2021 Telink Semiconductor (Shanghai) Co., Ltd. All rights reserved.
 *
 *           The information contained herein is confidential property of Telink
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *           Co., Ltd. and the licensee or the terms described here-in. This heading
 *           MUST NOT be removed from this file.
 *
 *           Licensee shall not delete, modify or alter (or permit any third party to delete, modify, or  
 *           alter) any information contained herein in whole or in part except as expressly authorized  
 *           by Telink semiconductor (shanghai) Co., Ltd. Otherwise, licensee shall be solely responsible  
 *           for any claim to the extent arising out of or relating to such deletion(s), modification(s)  
 *           or alteration(s).
 *
 *           Licensees are granted free, non-transferable use of the information in this
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/

#import <UIKit/UIKit.h>
#import "CustomAlert.h"

NS_ASSUME_NONNULL_BEGIN

typedef void(^AlertResult)(CustomAlert *alertView, BOOL isConfirm);
@interface CustomAlertView : UIView

- (instancetype)initWithTitle:(NSString *)title detail:(NSString *)detail itemArray:(NSArray <AlertItemModel *>*)itemArray leftBtnTitle:(NSString *)leftBtnTitle rightBtnTitle:(NSString *)rightBtnTitle alertResult:(AlertResult) alertResult;

- (void)showCustomAlertView;

@end

NS_ASSUME_NONNULL_END

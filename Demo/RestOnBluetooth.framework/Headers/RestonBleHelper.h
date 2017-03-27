﻿#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>

#define SLPRestonBleHelper [RestonBleHelper share]
#define SLPRestonBleRQHelper [RestonBLEHelper share].requestHelper


@class AllSleepData;
@class SDCCP;
@class BLEMatchHead;
@class UpdateSummary;
@class UpdateDetail;

typedef unsigned char byte;
typedef unsigned char *pbyte;

typedef void (^nothingBlock)();
typedef void (^iBlock)(NSInteger status);
typedef void (^iiBlock)(NSInteger count, NSInteger total);
typedef void (^strBlock)(NSString * str);
typedef void (^dictBlock)(NSDictionary * dict);
typedef void (^sleepdataBlock)(AllSleepData * sleepData);

//设备低电量通知(在需要提示低电量的View接收该通知)
#define BETTERYLOWNOTIFIY @"BETTERYLOWNOTIFIY"
//蓝牙类通知
#define kNotifyBluetoothPoweron @"kNotifyBluetoothPoweron" //系统蓝牙开启
#define kNotifyBluetoothPoweroff @"kNotifyBluetoothPoweroff"//系统蓝牙关闭
#define kNotifyBluetoothDisconnect @"kNotifyBluetoothDisconnect"//蓝牙连接断开
#define kNotifyBluetoothConnected @"kNotifyBluetoothConnected"//蓝牙已连接上设备



@interface RestonBleHelper:NSObject

@property(nonatomic,readonly)CBPeripheral *peripheral;//蓝牙服务
@property(nonatomic,readonly)CBCharacteristic *readCharactertic;//特征

/*
 share： 第一次调用时务必要调initWithPeripheral:readCharactertic:接口，用以初始化蓝牙的参数
 参数：
 peripheral 蓝牙服务
 readCharactertic 蓝牙读特征
 */
+(RestonBleHelper *)share;

//与share相对，销毁单例
+(void)deshare;

/*
 initWithPeripheral：初始化， 第一次调用share接口时务必要调用本接口，用以初始化蓝牙的参数
 参数：
 peripheral 蓝牙服务
 readCharactertic 蓝牙读特征
 */
- (void) initWithPeripheral:(CBPeripheral *)peripheral readCharactertic:(CBCharacteristic *)readCharactertic;

/**
 *  登录设备(与设备蓝牙连接上后,需要登录才能调用功能接口)
 *
 *  @param deviceID 设备ID
 *  @param userID   用户ID,用于区分数据归属哪个用户,若没有用户ID则填0
 *  @param timezone 时区(秒)
 *  @param success  登录成功
 *  @param failure  登录失败
 */
- (void)loginDeviceWithDeviceID:(NSString *)deviceID
                      andUserID:(int)userID
                    andTimeZone:(int)timezone
                        success:(void (^)(NSInteger status))success
                        failure:(void (^)(void))failure;

/*
 *  获取某一段时间内的所有历史数据
 *
 *  @param startTime     开始时间
 *  @param endTime       结束时间
 *  @param sumBlock      数据条数
 *  @param EachDataBlock 每下载一条数据都会回调一次,每次返回一条睡眠数据
 *  @param completion    所有数据已下载完毕时回调.
 */
-(void)getHistoryDataWithStartTime:(uint)startTime
                        andEndTime:(uint)endTime
                          sumBlock:(void (^)(NSInteger sum))sumBlock
                     EachDataBlock:(void (^)(AllSleepData *sleepData))EachDataBlock
                        completion:(void (^)(void))completion;




/* 获取硬件的版本信息
 
 参数：
 version 版本号
 */
-(void)getDeviceVersionWithSuccess:(void (^)(NSString *version))success
                           failure:(void (^)(void))failure;


/*
 setDeviceSampling：设置采集状态
 参数status值：
 0 设置停止
 1 设置开始
 
 返回值status值：
 1：设置失败
 0：设置成功
 */
-(void)setDeviceSamplingWithStatus:(int)status
                           success:(void (^)(NSInteger status))success
                           failure:(void (^)(void))failure;

/* 获取采集状态
 返回值status值：
 1：正在采集状态
 0：未处于采集状态
 */
-(void)getDeviceSamplingWithSuccess:(void (^)(NSInteger status))success
                            failure:(void (^)(void))failure;


/*开始获取实时数据
 
 返回数据结构如下:
 {
 awakeFlag = 0;  //  0:无效 1:清醒
 br = 0;         //  呼吸率
 hr = 0;         //  心率
 sleepFlag = 0;  //  0:无效 1:睡着
 status = 2;     //  0:一切正常 1:初始化状态 2:呼吸暂停 3	:心跳暂停 4:体动 5	:离床 6:翻身
 statusValue = 8;//  标记status状态的持续时间(秒)
 }
 */
-(void)startRealValueWithSuccess:(void (^)(NSDictionary *dict))success;

//停止获取实时数据
-(void)stopRealValue;

//获取电池信息
-(void)getDevicePowerStatusSuccess:(void (^)(NSInteger status))success
                           failure:(void (^)(void))failure;

/**
 *  设置自动采集时间点
 *
 *  @param valid   1:开启自动监测 0:关闭自动监测
 *  @param hour    时(24小时制)
 *  @param min     分
 *  @param array   重复日期(星期几)例:@[1,1,0,0,0,0,0] index0为周7,lastObject为周一, 这个数组表示周六和周七开启自动监测
 *  @param success 成功
 *  @param failure 失败
 */
-(void)setAutoGatherTimeDataValid:(int)valid
                             hour:(int)hour
                          minutes:(int)min
                            array:(NSMutableArray*)array
                          Success:(void (^)(void))success
                          failure:(void (^)(void))failure;

/**
 *  设置闹铃信息给RestON
 *
 *  @param valid   1:开启 0:关闭
 *  @param offset  唤醒时间范围(分钟)
 *  @param hour    时(24小时制)
 *  @param min     分
 *  @param weekArr 重复日期(星期几)例:@[1,1,0,0,0,0,0] index0为周7,lastObject为周一, 这个数组表示周六和周七开启闹铃功能
 *  @param success 成功
 *  @param failure 失败
 */
- (void)setAlarmInfoWithValid:(int)valid
                       offset:(int)offset
                         hour:(int)hour
                      minutes:(int)min
                      weekArr:(NSArray *)weekArr
                      Success:(void(^)(void))success
                      failure:(void(^)(void))failure;

/**
 *  获取RestOn密文ID
 *
 */
- (void)getRestonIDwithDeviceName:(NSString *)deviceName
                          Success:(void (^)(NSString *deviceID))success
                          failure:(void (^)(void))failure;


#pragma mark -Private
//保留，供BleManager调用
- (void)appendBytes:(byte *)buffer :(int)length;
- (void)initial;

@end

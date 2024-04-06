//
// Created by jjangchan on 2024/03/28.
//

#ifndef MAIN_CPP_ERRORS_H
#define MAIN_CPP_ERRORS_H

#include "flatjson.hpp"

namespace error{
namespace binance{}
//namespace coinone{}
}

/*************************************************************************************************/
// https://github.com/binance/binance-spot-api-docs/blob/master/errors.md
enum class binance_error : int{
    OK = 0
    ,UNKNOWN = -1000
    ,DISCONNECTED = -1001
    ,UNAUTHORIZED = -1002
    ,TOO_MANY_REQUESTS = -1003
    ,UNEXPECTED_RESP = -1006
    ,TIMEOUT = -1007
    ,UNKNOWN_ORDER_COMPOSITION = -1014
    ,TOO_MANY_ORDERS = -1015
    ,SERVICE_SHUTTING_DOWN = -1016
    ,UNSUPPORTED_OPERATION = -1020
    ,INVALID_TIMESTAMP = -1021
    ,INVALID_SIGNATURE = -1022
    ,ILLEGAL_CHARS = -1100
    ,TOO_MANY_PARAMETERS = -1101
    ,MANDATORY_PARAM_EMPTY_OR_MALFORMED = -1102
    ,UNKNOWN_PARAM = -1103
    ,UNREAD_PARAMETERS = -1104
    ,PARAM_EMPTY = -1105
    ,PARAM_NOT_REQUIRED = -1106
    ,BAD_PRECISION = -1111
    ,NO_DEPTH = -1112
    ,TIF_NOT_REQUIRED = -1114
    ,INVALID_TIF = -1115
    ,INVALID_ORDER_TYPE = -1116
    ,INVALID_SIDE = -1117
    ,EMPTY_NEW_CL_ORD_ID = -1118
    ,EMPTY_ORG_CL_ORD_ID = -1119
    ,BAD_INTERVAL = -1120
    ,BAD_SYMBOL = -1121
    ,INVALID_LISTEN_KEY = -1125
    ,MORE_THAN_XX_HOURS = -1127
    ,OPTIONAL_PARAMS_BAD_COMBO = -1128
    ,INVALID_PARAMETER = -1130
    ,NEW_ORDER_REJECTED = -2010
    ,CANCEL_REJECTED = -2011
    ,NO_SUCH_ORDER = -2013
    ,BAD_API_KEY_FMT = -2014
    ,REJECTED_MBX_KEY = -2015
    ,NO_TRADING_WINDOW = -2016
};


/*************************************************************************************************/
// https://docs.coinone.co.kr/docs/error-code
enum class coinone_error : int{
    BLOCKED_USER = 4
    ,NEED_PARAMETER = 8
    ,MISSING_TOKEN = 11
    ,INVALID_TOKEN = 12
    ,SERVICE_NOT_EXIST = 20
    ,NEED_REGISTER_API = 21
    ,SERVICE_NOT_APPROVED = 22
    ,INVALID_SECRET = 23
    ,INVALID_ID = 24
    ,REQTOKEN_NOT_EXIST = 25
    ,FAILED_DELETE_REQTOKEN = 26
    ,ACCTOKEN_NOT_EXIST = 27
    ,FAILED_DELETE_ACCTOKEN = 28
    ,FAILED_REFRESH_ACCTOKEN = 29
    ,INVALID_API_PERMISSION = 40
    ,NEED_KYC = 50
    ,INVALID_API = 51
    ,NEED_ACCOUNT_VERIFICATION = 52
    ,FAIL_2_FACTOR = 53
    ,INVALID_FORMAT = 101
    ,LACK_BALANCE = 103
    ,ORDER_NOT_EXIST = 104
    ,PRICE_NOT_CORRECT = 105
    ,PARAMETER_ERROR = 107
    ,UNKNOWN_SYMBOL = 108
    ,UNKNOWN_PAIR = 109
    ,DIFFERENCE_ORDER_CURRENT = 111
    ,QUANTITY_LOW = 113
    ,INVALID_ORDER_AMOUNT = 114
    ,EXCEEDED_MAX_QUANTITY = 115
    ,ALREADY_TRADED = 116
    ,ALREADY_CANCELED = 117
    ,ALREADY_SUBMITTED = 118
    ,V2_PAYLOAD_MISSING = 120
    ,V2_SIGNATURE_MISSING = 121
    ,V2_NONCE_MISSING = 122
    ,V2_SIGNATURE_NOT_CORRECT = 123
    ,INVALID_V2_NONCE1 = 130
    ,INVALID_V2_NONCE2 = 131
    ,NONCE_ALREADY_USED = 132
    ,NONCE_MUST_UUID = 133
    ,IV2_NOT_ACCEPTABLE = 151
    ,INVALID_ADDRESS = 152
    ,ADDRESS_DETECTED = 153
    ,ADDRESS_NOT_REGISTERED = 154
    ,REGISTER_DEPOSIT_ADDRESS = 155
    ,ADDRESS_NOT_EXIST = 156
    ,INSUFFICIENT_BALANCE = 157
    ,INVALID_WITHDRAWAL = 158
    ,NEED_MEMO = 159
    ,INVALID_ID_WITHDRAWAL = 160
    ,INVALID_ORDER_LIMIT1 = 161
    ,INVALID_ORDER_LIMIT2 = 162
    ,NO_POST_ONLY = 163
    ,NO_TRIGGER_PRICE = 164
    ,NO_AMOUNT = 165
    ,INVALID_ORDER_TYPE = 166
    ,INVALID_TRIGGER_PRICE = 167
    ,INVALID_WITHDRAWAL2 = 202
    ,INVALID_ORDER_INFO = 300
    ,INVALID_SELL_ORDER1 = 301
    ,INVALID_SELL_ORDER2 = 302
    ,INVALID_BUY_ORDER1 = 303
    ,INVALID_BUY_ORDER2 = 304
    ,INVALID_QUANTITY = 305
    ,INVALID_ORDERS_AMOUNT_MIN = 306
    ,INVALID_ORDERS_AMOUNT_MAX = 307
    ,PRICE_OUT_RANGE = 308
    ,QTY_OUT_RANGE = 309
    ,INVALID_PRICE_UNIT = 310
    ,INVALID_QTY_UNIT = 311
    ,DUPLICATED_USER_ID = 312
    ,INVALID_USER_ID1 = 313
    ,INVALID_USER_ID2 = 314
    ,NOT_SUPPORT_PORTFOLIO = 315
    ,SERVER_ERROR = 405
    ,INVALID_2FA = 777
    ,V2_ORDER_ERROR = 1201
    ,V21_ORDER_ERROR = 1202
    ,USER_NOT_FOUND = 1206
    ,WITHDRAWAL_SUSPENDED = 3001
    ,WITHDRAWAL_REJECTED = 3002
    ,WITHDRAWAL_LIMIT = 3003
    ,WITHDRAWAL_24_RESTRICTED = 3004
    ,TRY_PHONE_VERIFICATION = 3005
    ,ERROR_BALANCE = 3007
    ,ACCOUNT_DETECTED_FDS = 3009
    ,ACCOUNT_LOCKED = 3010
    ,WITHDRAWAL_RESTRICTED = 3011
};

/*************************************************************************************************/

enum class bithumb_error : int{

};

/*************************************************************************************************/

#endif //MAIN_CPP_ERRORS_H
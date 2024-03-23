#include "../include/main.h"
//传入用户名和第一次登录时间（字符串形式），生成token
int create_token(char *user,char**jwt)
{
    char key[200]={0};
    strcat(key,user);

    size_t jwt_length;//令牌的长度

    struct l8w8jwt_encoding_params params;//配置参数
    l8w8jwt_encoding_params_init(&params);//初始化

    params.alg = L8W8JWT_ALG_HS512;//签名使用的算法

    params.sub = "netdisk";//主题
    params.iss = "server";//签发人
    params.aud = "client";//受众

    params.iat = 0;//令牌的签发时间
    params.exp = 0x7fffffff;//令牌的过期时间（设置为永久有效）

    params.secret_key = (unsigned char*)key;//以用户名为密钥
    params.secret_key_length = strlen(key);//密钥的长度

    params.out = jwt;
    params.out_length = &jwt_length;

    l8w8jwt_encode(&params);//执行加密函数，取得token

    return 0;
}

int check_token(char *user,char *JWT,int *pcret)//传入用户名和第一次登录时间（字符串形式），用户携带的token,token一致返回0，不一致返回1；
{
    char key[200]={0};
    strcat(key,user);

    struct l8w8jwt_decoding_params params;//配置文件
    l8w8jwt_decoding_params_init(&params);//初始化

    params.alg = L8W8JWT_ALG_HS512;//签名使用的算法

    params.jwt = (char*)JWT;
    params.jwt_length = strlen(JWT);//获取令牌的长度

    params.verification_key = (unsigned char*)key;//以用户名作为密钥
    params.verification_key_length = strlen(key);//获得长度

    /* 
     * Not providing params.validate_iss_length makes it use strlen()
     * Only do this when using properly NUL-terminated C-strings! 
     */
    params.validate_iss = "server";//签发人与create token保持一致 
    params.validate_sub = "netdisk";//主题与create token保持一致

    /* Expiration validation set to false here only because the above example token is already expired! */
    //默认令牌永久有效，不需要设置
    //params.validate_exp = 0; 
    // params.exp_tolerance_seconds = 60;

    //params.validate_iat = 1;
    // params.iat_tolerance_seconds = 60;

    enum l8w8jwt_validation_result validation_result;

    int decode_result = l8w8jwt_decode(&params, &validation_result, NULL, NULL);//解码结果

    if (decode_result == L8W8JWT_SUCCESS && validation_result == L8W8JWT_VALID) 
    {
        *pcret=0;//比对token一致返回0；不一致返回1
    }
    else
    {
        *pcret=1;
    }
    return 0;
    /*
     * decode_result describes whether decoding/parsing the token succeeded or failed;
     * the output l8w8jwt_validation_result variable contains actual information about
     * JWT signature verification status and claims validation (e.g. expiration check).
     * 
     * If you need the claims, pass an (ideally stack pre-allocated) array of struct l8w8jwt_claim
     * instead of NULL,NULL into the corresponding l8w8jwt_decode() function parameters.
     * If that array is heap-allocated, remember to free it yourself!
     */
}

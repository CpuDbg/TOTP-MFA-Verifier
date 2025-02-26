# TOTP-MFA-Verifier
基于C语言实现的TOTP, 适用于阿里云MFA验证

阿里云的MFA（Multi-Factor Authentication，多因素认证）是一种安全机制，旨在通过增加额外的身份验证步骤来增强账号的安全性。MFA在用户名和密码的基础上，增加了第二层安全要素，通常是动态验证码或硬件密钥，以降低账号被盗的风险

MFA的算法
阿里云的MFA验证基于TOTP（Time-Based One-Time Password，基于时间的一次性密码）协议，这是一种广泛采用的多因素认证协议。TOTP算法是公开的，其核心原理是通过共享密钥和当前时间生成动态验证码

以后版本可以移植到STM32等单片机上...

<img width="318" alt="demo_phone" src="https://github.com/user-attachments/assets/bb26e550-ed04-43ee-98ea-373bfe1bfe74" />  
![demo_pc](https://github.com/user-attachments/assets/0b8509f3-a860-407f-81a2-09cc12951e4c)


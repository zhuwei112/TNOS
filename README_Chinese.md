 tnos创建者说明:
    创始人:朱巍. (wilson zhu)
 
 总体说明:
    1. tnos 是一个基于时间片轮询和优先级的微型实时操作系统. 任务延迟能精确到1ms.
              如:tnos_delay_ms(13), tnos_delay_ms(1),都能精确延迟.
    2. 互斥锁(tnos_mutex). 信号量(tnos_sem),消息队列(tnos_msgq),循环缓冲(tnos_msgcyc),
                都能通过tnos_select 同时监听. 也可以单独等待. 所有IPC通过信号完成内核任务的唤醒.        
    3. 运行中可修改优先级和时间片.   
    4. 能打印堆栈检查和cpu占用率.       
    5. arm堆栈说明: 使用2种堆栈. 任务用从堆栈指针, 中断用主堆栈(中断堆栈不占用任务空间!!!).
    6. 中断嵌套说明: 必须支持中断嵌套,否则禁止中断不能正常运行.
    7. 只针对于32位cpu.程序需要/1000变ms,建议有硬件除法器.  
    8. xtimer 定时器能实现 延迟ms和延迟us, 开机后68年溢出,是理想的定时控制. 
     
任务介绍:
     任务分为:等待运行,就绪, 运行,休眠  4种状态: 状态图如下:
           
                                                                                           等待运行     
                               |    ↑
                              |      | 
                             |        |
    就绪队列为空,重新分配时间片            |          |   任务时间片消耗完    
     且全部等待运行改为就绪态              |             |     
                         |                |
                        |                  |
                       |    高和同优先级打断        |
                      |      且有时间片                        |     
                     ↓   <-----------------   |                   
                                                    就绪态          -----------------> 运行态      
                 ↑    |    高和同优先级调度
                 |    |
                 |    |                                                              
                 任务延时到达    |    | 等待信号延迟   
                或者收到信号     |    | 等待信号
                 |    |
                 |    ↓
                                                   休眠 态
             
                           
    
  
  
外设占用:    
 使用1个定时器: 一个普通定时器,完成计时(ttimer)和定时(系统ms延时). 时间最小精度为ms,定时最大时长为2秒
    使用1个串口: 调试打印输出.程序建议不用printf, 使用 xprintf和xprintf_w代替!!!
               xprintf 能中断打印不占用cpu时间(中断输出需要额外内存),  
               xprintf_w直接打印占用cpu时间(轮询串口输出).
              

                                                                    
                  
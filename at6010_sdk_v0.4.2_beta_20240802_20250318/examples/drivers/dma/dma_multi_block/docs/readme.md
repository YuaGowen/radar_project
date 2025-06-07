# DMA Multi-block 传输之 linked list

首先需知晓此 DMA 支持 10 种传输类型，参见 `dma_transfer_type_t`。

这里以 `DMA_TRANSFER_TYPE_ROW10` 为例，DMA 支持多块传输，且源地址和目的地址都由 linked list 指定。

# Linked-list 形式的多块传输

linked-list 传输，必须要了解结构体 `dma_link_list_t` 结构体，如下所示：

```C
typedef struct _dma_link_list_t {
	uint32_t                    src_addr;   /*!< source address, could be memory buffer address or hardware data register address      */

	uint32_t                    dst_addr;   /*!< destination address, could be memory buffer address or hardware data register address */

	struct _dma_link_list_t    *llp_next;   /*!< address of next linked list                                                           */

	dma_ctrl_t                  control;    /*!< control information for one block transfer                                            */

	uint32_t                    src_status; /*!< write back for source status                                                          */

	uint32_t                    dst_status; /*!< write back for destination status                                                     */
} dma_link_list_t;
```

其中 `src_addr` 指定当前块的源地址，`dst_addr` 指定当前块的目的地址。源地址和目的地址既可以是内存缓冲区地址，也可以是外设数据寄存器地址，视情况设置合理值。

其中 `llp_next` 是指向下一个链表地址。可以手动指定，也可以调用函数 `hal_dma_lli_struct_init()` 初始化链表数组，自动把 `llp_next` 指向下一个元素地址。最后一个元素的 `llp_next` 指向 **NULL**。

先介绍 `src_status` 和 `dst_status`，它们分别表示源状态和目的状态，在DMA传输一块后由DMA控制器去取回源和目的状态，分别保存在这两个成员上。一般情况可以忽略这两个成员。

其中 `control` 是 DMA 控制参数，它的定义如下所示：

```C
typedef struct _dma_ctrl_t {
	u32 int_en         : 1; /*!< (USER IGNORE HERE) Interrupt Enable (global mask)                */

	u32 dst_tr_width   : 3; /*!< Destination Transfer Width  @ref DMA_Transfer_Width              */
	u32 src_tr_width   : 3; /*!< Source Transfer Width       @ref DMA_Transfer_Width              */

	u32 dst_addr_inc   : 2; /*!< Destination Address Increment  @ref DMA_Addr_Increment_Type      */
	u32 src_addr_inc   : 2; /*!< Source Address Increment       @ref DMA_Addr_Increment_Type      */

	u32 dst_burst_size : 3; /*!< Destination Burst Transaction Length @ref DMA_Burst_Size         */
	u32 src_burst_size : 3; /*!< Source Burst Transaction Length      @ref DMA_Burst_Size         */

	u32 src_gather_en  : 1; /*!< Source gather enable        @ref DMA_Gather_Management           */
	u32 dst_scatter_en : 1; /*!< Destination scatter enable  @ref DMA_Scatter_Management          */

	u32 rsvd1          : 1;

	u32 tt_fc          : 3; /*!< Transfer Type and Flow Control @ref DMA_Flow_Control             */

	u32 rsvd2          : 4;

	u32 dst_llp_en     : 1; /*!< (USER IGNORE HERE) Linked List enable on destination side        */
	u32 src_llp_en     : 1; /*!< (USER IGNORE HERE) Linked List enable on source side             */

	u32 rsvd3          : 3;

	u32 block_size     : 12; /*!< (USER IGNORE HERE) Block Transfer Size                          */
	u32 done           : 1;  /*!< (USER IGNORE HERE) Block Transfer Done                          */

	u32 rsvd4          : 19;
} dma_ctrl_t;
```

- `int_en` 表示此DMA通道上的中断总开关。值为0表示关闭此通道上的所有中断，值为1表示允许所有中断发生。（还需要使能各个中断的掩码才能真正报告各个中断状态）；
- `dst_tr_width` 和 `src_tr_width` 表示目的传输宽度、源传输宽度。即DMA传输每个元素的位宽度，参见 `DMA_Transfer_Width`，例如传输位宽度为8比特，16比特或者32比特，甚至是64比特等；
- `dst_addr_inc` 和 `src_addr_inc` 表示目的地址、源地址增长类型。可选值有3种，参见 `DMA_Addr_Increment_Type`，分别表示地址增大、地址减小，地址不变。如果地址是外设数据寄存器，就必须设置地址为不变的类型；
- `dst_burst_size` 和 `src_burst_size` 表示目的、源的一次 burst 传输元素个数。
- `src_gather_en` 和 `dst_scatter_en` 表示 Source Gather 特性使能位，Destination Scatter 特性使能位。Source Gather 特性使能的话需要搭配 `source gather count` 和 `source gather interval` 两个参数；而 Destination Scatter 特性使能的话需要搭配 `destination scatter count` 和 `destination scatter interval` 两个参数。此处暂时不涉及这两个特性。
- `tt_fc` 指块传输方向和流量控制器。参见 `DMA_Flow_Control`类型。此处演示内存到内存传输，以 DMA 作为流量控制器，所以值设为 `DMA_TT_M2M_FC_DMA` 数值为0；
- `dst_llp_en` 和 `src_llp_en` 分别表示目的是否使用链表，源是否使用链表。此示例在源和目的都是用链表，所以两个参数都设置为1；
- `block_size` 表示传输块的元素个数。以上面的传输宽度为 `DMA_TR_WIDTH_8_BITS` 为例，一块表示 8bit 即1个字节；
- `done` 是状态位，初始值为0，表示块传输没有结束；当块传输结束，DMA控制器自动把 `done` 设置为1，用户可以轮询此成员来判断块传输是否结束。


# 示例工程

使用 DMA 驱动的步骤：

1. 使能 DMA 时钟；
2. 初始化 linked list 结构体，例如调用函数 `hal_dma_lli_struct_init()` 初始化链表，自动链接每一个元素；
3. 初始化 DMA handle 结构体中的 init 结构体，调用函数 `hal_dma_init_struct_init()`，注意必须设置传输类型为 `DMA_TRANSFER_TYPE_ROW10`；
4. 关联链表到 DMA handle.init，例如 `hdmax.init.llp_loc = (uint32)&lli[0]`；
5. 调用函数 `hal_dma_init()` 初始化 DMA ；
6. 注册 DMA 两个回调函数，一个是DMA传输完成回调函数，一个是DMA发生错误的回调函数；
7. NVIC 使能 DMA 中断号；
8. 调用 `hal_dma_start_it()` 启动DMA传输；
9. 在中断服务程序 `DMA_Handler()` 中调用 `hal_dma_irq_handler(&hdmax)`，让 DMA HAL 驱动处理块并自动停止传输；
10. DMA HAL 驱动会调用用户注册的回调函数，例如这里的回调函数只是改变两个 flag 值；
11. 等待传输完成，处理数据；

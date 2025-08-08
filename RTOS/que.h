/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   que.h 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief  que 
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#ifndef __QUE_H__
#define __QUE_H__

/*--------------------------------------------------------------------------------------*/
/*! @brief  que struct
 */
typedef struct _que_t {
    struct _que_t *p_prev;
    struct _que_t *p_next;
} que_t;

/*--------------------------------------------------------------------------------------*/
/*! @brief  
 */
// insert to que top
#define enque_top(p_root, p_elm)    (enque(p_root, (p_root)->p_next, p_elm))
// insert to qut last
#define enque_last(p_root, p_elm)   (enque((p_root)->p_prev, p_root, p_elm))
// insert get que top
#define que_top(p_root)             (deque(p_root))

/*--------------------------------------------------------------------------------------*/
/*! @brief  
 */
void que_init(que_t *p_root);
void enque(que_t *p_prev, que_t *p_next, que_t *p_elm);
void deque(que_t *p_elm);

#endif

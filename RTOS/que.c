/*--------------------------------------------------------------------------------------*/
/*!
 *  @file   que.c 
 *  @date   2025.xx.xx
 *  @author mrzm99
 *  @brief  que 
 *  @note
 */
/*--------------------------------------------------------------------------------------*/
#include "que.h"
#include <stdio.h>

/*--------------------------------------------------------------------------------------*/
/*! @brief  
 */
void que_init(que_t *p_root)
{
    p_root->p_prev = p_root;
    p_root->p_next = p_root;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  
 */
void enque(que_t *p_prev, que_t *p_next, que_t *p_elm)
{
    p_prev->p_next = p_elm;
    p_next->p_prev = p_elm;

    p_elm->p_prev = p_prev;
    p_elm->p_next = p_next;
}

/*--------------------------------------------------------------------------------------*/
/*! @brief  
 */
void deque(que_t *p_elm)
{
    que_t *p_prev = p_elm->p_prev;
    que_t *p_next = p_elm->p_next;

    if ((p_prev != NULL) && (p_next != NULL)) {
        p_prev->p_next = p_next;
        p_next->p_prev = p_prev;
        p_elm->p_prev = NULL;
        p_elm->p_next = NULL;
    }
}

#include "ringbuffer.h"

/* Create a new ring buffer */
ring_buffer *ring_buffer_new()
{
    ring_buffer *buffer = (ring_buffer *)malloc(sizeof(ring_buffer));
    /* Return NULL if fail to allocate memory */
    if (buffer == NULL)
        return NULL;
    buffer->capacity = RING_BUFFER_INIT_SIZE;
    /* Starting position */
    buffer->read_pos = 0;
    /* Ending position */
    buffer->write_pos = 0;
    /* Return NULL if fail to allocate memory */
    buffer->content = (int *)calloc(buffer->capacity, sizeof(int));
    if (buffer->content == NULL)
        return NULL;
    return buffer;
}

/* Destroy a ring buffer */
void ring_buffer_destroy(ring_buffer **buffer)
{
    /* Exclude potential invalid operations */
    if (buffer == NULL || *buffer == NULL)
        return;
    /* free the vector */
    if ((*buffer)->content != NULL)
        free((*buffer)->content);
    /* free the buffer itself */
    free(*buffer);
    *buffer = NULL;
}

/* Check if buffer is full */
static bool ring_buffer_is_full(const ring_buffer *buffer)
{
    /* Exclude potential invalid operations */
    if (buffer == NULL)
        return false;
    else
    {
        /* Return true if number of elements equals capacity-1 */
        size_t cnt = (buffer->write_pos - buffer->read_pos + buffer->capacity) % buffer->capacity;
        return cnt == buffer->capacity - 1;
    }
}

/* Check if buffer is empty */
bool ring_buffer_is_empty(const ring_buffer *buffer)
{
    /* Exclude potential invalid operations */
    if (buffer == NULL)
        return true;
    return buffer->write_pos == buffer->read_pos;
}

/* Read or Dequeue operation */
bool ring_buffer_read(ring_buffer *buffer, int *data)
{
    /* Exclude potential invalid operations */
    if (data == NULL || buffer == NULL || ring_buffer_is_empty(buffer))
        return false;
    /* Load data from the buffer */
    *data = buffer->content[buffer->read_pos++];
    /* Dequeue the data and return true */
    buffer->read_pos %= buffer->capacity;
    return true;
}

/* Copy the stored data from the old to the new in correct order */
static void copy_data(ring_buffer *buffer, size_t new_capacity)
{
    int i = 0;
    /* Count the number of elements */
    int s = buffer->read_pos;
    int t = buffer->write_pos;
    size_t cnt = (t - s + buffer->capacity) % buffer->capacity;
    /* Create a temporary array to hold the arranged data */
    int *new_content = (int *)malloc(new_capacity * sizeof(int));
    /* Move elements from old array to new array */
    while (!ring_buffer_is_empty(buffer))
        ring_buffer_read(buffer, &new_content[i++]);
    /* The origin vector should be freed after being useless */
    free(buffer->content);
    /* Buffer update */
    buffer->content = new_content;
    buffer->capacity = new_capacity;
    buffer->read_pos = 0;
    buffer->write_pos = cnt;
}

/* Expand the capacity of the buffer */
static void expand_capacity(ring_buffer *buffer)
{
    /* Scenario 1: size doesn't exceed 1024*/
    if (buffer->capacity < 1024)
        /* Arrange data in correct order */
        copy_data(buffer, buffer->capacity * RING_BUFFER_GROW_FACTOR1);
    /* Scenario 2: size exceeds 1024 */
    else
        /* Arrange data in correct order */
        copy_data(buffer, buffer->capacity * RING_BUFFER_GROW_FACTOR2);
}

/* Write data to the buffer */
bool ring_buffer_write(ring_buffer *buffer, const int data)
{
    /* Exclude potential invalid operations */
    if (buffer == NULL)
        return false;
    /* Expand the capacity if buffer is full */
    if (ring_buffer_is_full(buffer))
        expand_capacity(buffer);
    /* Write data to the buffer */
    buffer->content[buffer->write_pos++] = data;
    buffer->write_pos %= buffer->capacity;
    return true;
}

/* Read multi-data from buffer */
bool ring_buffer_read_multi(ring_buffer *buffer, size_t rdsize, int *data)
{
    /* Exclude potential invalid operations */
    if (rdsize == 0 || data == NULL || buffer == NULL || ring_buffer_is_empty(buffer))
        return false;
    else
    {
        int i = 0;
        /* Count the number of elements */
        size_t cnt = (buffer->write_pos - buffer->read_pos + buffer->capacity) % buffer->capacity;
        /* Set a flag indicating whether rdsize exceeds the actual number of stored elements */
        int flag = rdsize <= cnt ? 1 : 0;
        /* Return if elements are not enough */
        if (flag == 0)
            return false;
        /* Dequeue elements from buffer and put them into data */
        while (rdsize--)
        {
            data[i++] = buffer->content[buffer->read_pos++];
            buffer->read_pos %= buffer->capacity;
        }
        return true;
    }
}

/* Write multi-data to buffer */
bool ring_buffer_write_multi(ring_buffer *buffer, size_t wrtsize, const int *data)
{
    /* Exclude potential invalid operations */
    if (buffer == NULL || data == NULL || wrtsize == 0)
        return false;
    /* One-by-one write-in process */
    else
    {
        size_t i;
        for (i = 0; i < wrtsize; i++)
        {
            /* If the buffer is already full, enlarge its capacity */
            if (ring_buffer_is_full(buffer))
                expand_capacity(buffer);
            /* Write elements into buffer */
            buffer->content[buffer->write_pos++] = data[i];
            buffer->write_pos %= buffer->capacity;
        }
        return true;
    }
}

/* Map function implementation */
bool ring_buffer_map(ring_buffer *buffer, map_func func)
{
    /* Exclude potential invalid operations */
    if (buffer == NULL || func == NULL)
        return false;
    else
    {
        /* Fetch two endings */
        int s = buffer->read_pos;
        int t = buffer->write_pos;
        while (s != t)
        {
            /* Traverse the buffer array and apply "func" to each element */
            buffer->content[s] = func(buffer->content[s]);
            s++;
            s %= buffer->capacity;
        }
        return true;
    }
}
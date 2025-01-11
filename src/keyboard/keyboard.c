#include "keyboard.h"
#include "status.h"
#include "task/process.h"
#include "classic.h"

static struct Keyboard *keyboard_head;
static struct Keyboard *keyboard_tail;

int keyboard_insert(struct Keyboard *keyboard)
{
    int res = 0;
    if (!keyboard->init)
    {
        return -EINARG;
    }
    if (keyboard_tail)
    {
        keyboard_tail->next = keyboard;
        keyboard_tail = keyboard;
    }
    else
    {
        keyboard_tail = keyboard;
        keyboard_head = keyboard;
    }
    res = keyboard->init();
    return res;
}

void keyboard_init()
{
    keyboard_insert(get_classic_keyboard());
}

static int keyboard_get_tail_index(struct KeyboardBuffer buffer)
{
    return buffer.tail % sizeof(buffer.buffer);
}

static int keyboard_get_head_index(struct KeyboardBuffer buffer)
{
    return buffer.head % sizeof(buffer.buffer);
}

void keyboard_push(char c)
{
    struct Process *process = get_current_process();
    if (!process)
    {
        return;
    }
    if (c == 0)
    {
        return;
    }
    int index = keyboard_get_tail_index(process->keyboard_buffer);
    process->keyboard_buffer.buffer[index] = c;
    process->keyboard_buffer.tail++;
}

char keyboard_pop()
{
    struct Task *task = get_current_task();
    if (!task)
    {
        return 0;
    }
    int index = keyboard_get_head_index(task->process->keyboard_buffer);
    char res = task->process->keyboard_buffer.buffer[index];
    if (res == 0x00)
    {
        return 0;
    }
    task->process->keyboard_buffer.head++;
    return res;
}
void keyboard_backspace(struct KeyboardBuffer *buffer)
{
    buffer->tail--;
    int index = keyboard_get_tail_index(*buffer);
    buffer->buffer[index] = 0x00;
}
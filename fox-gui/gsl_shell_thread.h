#ifndef FOXGUI_LUA_ENGINE_H
#define FOXGUI_LUA_ENGINE_H

extern "C" {
#include "lua.h"
}

#include "gsl_shell_interp.h"
#include "pthreadpp.h"
#include "redirect.h"
#include "str.h"

class gsl_shell_thread : public gsl_shell
{
public:
    enum engine_status_e { starting, ready, busy, terminated };
    enum request_e { no_request = 0, exit_request, restart_request };
    enum { eot_character = 0x04 };

    gsl_shell_thread();
    ~gsl_shell_thread();

    void set_request(request_e req);

    void input(const char* line);
    void start();
    void run();

    virtual void before_eval() { }
    virtual void restart_callback() { }
    virtual void quit_callback() { }

    void lock()
    {
        pthread_mutex_lock(&this->exec_mutex);
    }
    void unlock()
    {
        pthread_mutex_unlock(&this->exec_mutex);
    }

    int read(char* buffer, unsigned buffer_size);

    int eval_status() const
    {
        return m_eval_status;
    }
    pthread::mutex& eval_mutex()
    {
        return m_eval;
    }

private:
    pthread_t m_thread;
    engine_status_e m_status;
    stdout_redirect m_redirect;
    pthread::cond m_eval;
    str m_line_pending;
    int m_eval_status;
    request_e m_request;
};

#endif

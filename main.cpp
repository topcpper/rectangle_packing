/*************************************************************
*
*main.cpp: the main cpp
*auther     : caowg
*Written on : 03/17/14.
*
*************************************************************/

#include <iostream>
#include "rec_packing.H"


using namespace std;

vector<Hline> g_v_hline; // 所有水平线
vector<Vline> g_v_vline ; // 所有垂直线
vector<rectangle> g_v_rec_undo ; // 未处理的木块
vector<rectangle> g_v_rec_done ; // 已经处理的小木块
vector<action_space> g_v_as ; // 动作空间
set<conner> g_s_conner; // 所有的实角


const int MAX = 999999;
const int MIN = -99999;


// 选择当前最优的放置
bool chose_as_rec(vector<rectangle>::iterator & i2chonse_rec,
                  vector<action_space>::iterator & i2chonse_as);

// 更新动作空间
void update_action_space(vector<rectangle>::iterator i2chonse_rec
                         vector<action_space>::iterator i2chonse_as );

// 更新线段
void update_line(vector<rectangle>::iterator i2chonse_rec);



// 计算穴度
// 确保不该边 i2rec 对应的值
void calculate_fd(vector<rectangle>:: const_iterator i2rec,
                  vector<action_space>::const_iterator i2as,
                  fit_degree & fd);




int main(int arg ,char *arv[])
{
        
}

// 选择动作空间和小木块
bool chose_as_rec(vector<rectangle>::iterator & i2chonse_rec,
                  vector<action_space>::iterator & i2chonse_as)
{
    if (g_v_rec_undo.size() == 0)
        return false;
    
    vector<rectangle>::iterator i2rec = g_v_rec_undo.begin();
    vector<action_space>::iterator i2as = g_v_as.begin();
    vector<rectangle>::iterator i2rec_chosen = g_v_rec_undo.begin();
    vector<action_space>::iterator i2as_chosen = g_v_as.begin();

    rectangle rec_chosen = g_v_rec_undo[0];
    fit_degree fd;
    fit_degree max_fd ;
    bool finded = 0;
    
    for (; i2as!= g_v_as.end() ; i2rec++) // 迭代动作空间
    {
        for (; i2rec != g_v_rec_undo.end() ; ++i2rec) // 迭代外部矩形块
        {
            if(!max_fd_of8values(i2rec,i2as,fd))
                continue;
            finded = 1;
            if(max_fd < fd || (fd == max_fd && *i2rec > rec_chosen) )
            {
                rec_chosen = *i2rec ;  // 由于小方块在迭代过程中会被改变，所以保存当前最优解的值
                                        // 坐标，放置方式等会改变
                i2rec_chosen = i2rec; // 指向当前最优解的那个小方块
                i2as_chosen = i2as ;
                max_fd = fd;
            }
        }
    }
    if (finded)
    {
        *i2rec_chosen  = rec_chosen ; // 将最优解的那个小方块设置为其最优值
        i2as->place_type = i2rec_chosen->place_type;
    }
    return finded ;
}

// 计算贴边数
int calculate_fd_k(vector<rectangle>::const_iterator i2rec,
                          vector<action_space>::const_iterator i2as)
{
    int fd_k = 0 ;
    if(i2rec->width == i2as->width )
        fd_k += 2;
    else
        fd_k += 1 ;
    if(i2rec->height == i2as->height )
        fd_k += 2;
    else
        fd_k += 1 ;
    return fd_k ;
}

// 计算平整度
// 没有采用剩余动作个数
double calculate_fd_s(vector<rectangle>::const_iterator i2rec,
                          vector<action_space>::const_iterator i2as)
{
    double di = 0 ;
    di = min(i2as->get_width - i2rec->get_width,i2as->get_height - i2rec->get_height);
    return di/(i2rec->get_width + i2rec->get_height);
}


// 计算它贴边数
int  calculate_fd_p(vector<rectangle>::const_iterator i2rec,
                          vector<action_space>::const_iterator i2as)
{
    int fd_p = 0;
    Hline top_line(i2rec->left_top(),i2rec->right_top());
    Hline bottle_line(i2rec->left_bottle,i2rec->right_bottle());
    Vline left_line(i2rec->left_bottle,i2rec->left_top());
    Vline left_line(i2rec->right_bottle(),i2rec->right_top());
    
    vector<Hline>::iterator ihline;
    vector<Vline>::iterator ivline;
    for (ihline = g_v_hline.begin(); ihline != g_v_hline.end(); ++ihline)
    {
        if(top_line.conflict(*ihline))
            fd_p++;
        if (bottle_line.conflict(*ihline))
            fd_p++;
    }

    for (ivline = g_v_vline.begin(); ivline != g_v_vline.end(); ++ivline)
    {
        if(left_line.conflict(*ivline))
            fd_p++;
        if (right_line.conflict(*ivline))
            fd_p++;
    }
    return fd_p;
}



// 计算当前放置下的fd
void calculate_fd(vector<rectangle>:: const_iterator i2rec,
                  vector<action_space>::const_iterator i2as,
                  fit_degree & fd)
{
    fd.k = calculate_fd_k(i2rec,i2as);
    fd.s = calculate_fd_s(i2rec,i2as);
    fd.p = calculate_fd_p(i2rec,i2as);
}

// 求出木块 i2rec 在动作空间i2as的4个位置中最忧位置
// 并且 i2rec 的值已经被更新
// 4 个位置是因为没有旋转
void  max_fd_of4values(const vector<rectangle>:: iterator & i2rec,
            const vector<action_space>::iterator & i2as,
            fit_degree & fd)
{
    fit_degree fd_max ; // 本动作空间针对当前木块目前最大的fd
    rectangle rec_op = *i2rec; // 当前最优的小木块

    // left bottle
    i2rec->set_ordinate_lb(i2as->left_bottle);
    calculate_fd(i2rec,i2as,fd_max);
    i2as->place_type = LEFT_BOTTLE ;
    rec_op = *i2rec;

    // left top
    i2rec->set_ordinate_lt(i2as->left_top() );
    calculate_fd(i2rec,i2as,fd);
    if (fd_max < fd || (fd_max == fd &&  rec_op < *i2rec ) )
    {
        fd_max = fd ;
        rec_op = *i2rec;
        i2as->place_type = LEFT_TOP ;
    }
        
    // right bottle
    i2rec->set_ordinate_rb(i2as->right_bottle() );
    calculate_fd(i2rec,i2as,fd);
    if (fd_max < fd || (fd_max == fd &&  rec_op < *i2rec ) )
    {
        fd_max = fd ;
        rec_op = *i2rec;
        i2as->place_type = RIGHT_BOTTLE ;
    }

    // right top
    i2rec->set_ordinate_rt(i2as->right_top() );
    calculate_fd(i2rec,i2as,fd);
    if (fd_max < fd || (fd_max == fd &&  rec_op < *i2rec ) )
    {
        fd_max = fd ;
        rec_op = *i2rec;
        i2as->place_type = RIGHT_TOP ;
    }
    // 根据放置方式，重新定小方块的坐标
    switch( i2as->place_type)
    {
    case LEFT_BOTTLE:
        i2rec->set_ordinate_lb(i2as->left_bottle);break;
    case LEFT_TOP:
        i2rec->set_ordinate_lt(i2as->left_top() );break;
    case RIGHT_BOTTLE:
        i2rec->set_ordinate_rb(i2as->right_bottle() );break;
    case RIGHT_TOP:
        i2rec->set_ordinate_rt(i2as->right_top() );break;
    default:
        cout<<"error";
    }
}

// 计算小矩形在动作空间 i2as 四个角的fd，包括水平和垂直方向
// 也即有4*2 个值选取
bool max_fd_of8values(const vector<rectangle>:: iterator & i2rec,
                      const vector<action_space>::iterator & i2as,
            fit_degree & fd)
{
    // 放不下,则直接返回false
    if(i2rec->width > i2as->width ||
        i2rec->height > i2as->height )
        return 0;
    
    max_fd_of4values(i2rec , i2as, fd);
    i2rec->rec_reverse(); // 宽高互换
    fit_degree fd_reverse ;
    rectangle rec_un_reverse(*i2rec);
    max_fd_of4values(i2rec , i2as, fd_reverse);
    if (fd < fd_reverse ) // 如果reverse后和之前 fd相同，则优先考虑未reverse的情况
    {
        fd = fd_reverse ;
    }
    else
    {
        i2rec->rec_reverse(); // 重新恢复转置
        *i2rec = rec_un_reverse;
        i2as->place_type = i2rec->place_type;
    }
    return 1 ;
}


// 左下角算法
void conner2as_lb(vector<rectangle>:: iterator i2chonse_rec,const conner & lb_conner)
{
    
    
    //a1 左下角往上，找到新动作右上角y坐标
    int as_top_y = MAX;
    for (vector<Hline>::iterator it = g_v_hline.begin() ;  it != g_v_hline.end(); ++it)
    {
        // 寻找满足条件的y坐标的最小的
        if (it->pt_left.x <= lb_conner.x && it->pt_right.x > lb_conner.x
            && it->get_y() > lb_conner.y)
        {
            if(as_top_y > it->get_y() )
                as_top_y = it->get_y();
        }
    }
    
    //a2 寻找右上角的x坐标
    int as_right_x = MAX ;
    for (vector<Vline>::iterator it = g_v_vline.begin() ; it != g_v_vline.end() ; it++)
    {
        // 寻找满足条件的x坐标最小的
        if (it->get_x() > lb_conner.x && it->pt_top.y > lb_conner.y
            && it->pt_bottle.y < as_top_y)
        {
            if (as_right_x > it->get_x())
                as_right_x = it->get_x();
        }
    }
    action_space as(lb_conner,as_right_x - lb_conner.x, as_top_y - lb_conner.y);
    if (find(g_v_as.begin(), g_v_as_.end(), as) == g_v_as.end())
        g_v_as.push_back(as);
    
    //b1  左下角往上，寻找右上角的x坐标
    as_right_x = MAX ;
    as_top_y = MAX ;
    for (vector<Vline>::iterator it = g_v_vline.begin() ; it != g_v_vline.end() ; it++)
    {
        // 寻找满足条件的x坐标最小的
        if (it->get_x() > lb_conner.x && it->pt_bottle.y <= lb_conner.y
            && it->pt_top.y > lb_conner.y)
        {
            if (as_right_x > it_get_x() )
                as_right_x = it->get_x();
        }
    }

    // b2 寻找右上角的y坐标
    for (vector<Hline>::iterator it = g_v_hline.begin() ;  it != g_v_hline.end(); ++it)
    {
        if (it->get_y() > lb_conner.y && it->pt_right.x > lb_conner.x
            && it->pt_left.x < as_right_x)
        {
            if(as_top_y > it->get_y() )
                as_top_y = it->get_y();
        }
    }

    action_space as2(lb_conner,as_right_x - lb_conner.x, as_top_y - lb_conner.y);
    if (find(g_v_as.begin(), g_v_as_.end(), as2) == g_v_as.end())
        g_v_as.push_back(as2);
}

// 左上角算法
void conner2as_lt(vector<rectangle>:: iterator i2chonse_rec,const conner & lt_conner)
{
    int as_bottle_y = MIN;
    int as_right_x = MAX;
    // a1 先向下扩展，寻找下边界,即动作空间的右下角的y坐标
    for (vector<Hline>::iterator it = g_v_hline.begin() ;  it != g_v_hline.end(); ++it)
    {
        // 寻找满足条件的y坐标的最大的
        if (it->get_y() < lt_conner.y && it->pt_left.x <= lt_conner.x
            && it->pt_right.x > lt_conner.x)
        {
            if (as_bottle_y < it->get_y() )
                as_bottle_y = it->get_y();
        }
    }

    // a2 寻找右边界，即右下角的x坐标
    for (vector<Vline>::iterator it = g_v_vline.begin() ; it != g_v_vline.end() ; it++)
    {
        // 寻找满足条件的x坐标最小的
        if (it->get_x() > lt_conner.x && it->pt_bottle.y < lt_conner.y
            && it->pt_top.y > as_bottle.y)
        {
            if (as_right_x > it->get_x() )
                as_right_x = it->get_x();
        }
    }

    action_space as(conner(lt_conner.x, as_bottle_y),as_right_x - lt_conner.x,
                    lt_conner.y - as_bottle_y );
    if (find(g_v_as.begin(), g_v_as_.end(), as) == g_v_as.end())
        g_v_as.push_back(as);

    // b1 先向右扩展，寻找右边界，即动作空间的右下角的x坐标
    as_bottle_y = MIN;
    as_right_x = MAX;
    for (vector<Vline>::iterator it = g_v_vline.begin() ; it != g_v_vline.end() ; it++)
    {
        // 寻找满足条件的x坐标最小的
        if (it->get_x() > lt_conner.x && it->pt_top.y >= lt_conner.y
            && it->pt_bottle.y < lt_conner.y )
        {
            if (as_right_x > it->get_x() )
                as_right_x = it->get_x();
        }
    }

    // b2 下边界，寻找右下角的y坐标
    for (vector<Hline>::iterator it = g_v_hline.begin() ;  it != g_v_hline.end(); ++it)
    {
        // 寻找满足条件的y坐标的最大的
        if (it->get_y() < lb_conner.y && it->pt_right.x > lt_conner.x
            && it->pt_left.x < as_right_x)
        {
            if(as_bottle_y < it->get_y() )
                as_bottle_y = it->get_y();
        }
    }
    action_space as2(conner(lt_conner.x, as_bottle_y),as_right_x - lt_conner.x,
                    lt_conner.y - as_bottle_y );
    if (find(g_v_as.begin(), g_v_as_.end(), as2) == g_v_as.end())
        g_v_as.push_back(as2);
}

// 右上角算法
void conner2as_rt(vector<rectangle>:: iterator i2chonse_rec,const conner & rt_conner)
{
    int as_bottle_y = MIN ;
    int as_left_x = MIN ;

    // a1 先向下扩展，寻找下边界，即左下角的y坐标
    for (vector<Hline>::iterator it = g_v_hline.begin() ;  it != g_v_hline.end(); ++it)
    {
        // 寻找满足条件的y坐标的最大的
        if (it->get_y() < rt_conner.y && it->pt_left.x < rt_conner.x
            && it->pt_right.x >= rt_conner.x)
        {
            if (as_bottle_y < it->get_y() )
                as_bottle_y = it->get_y();
        }
    }
    // a2 寻找左边界，即左下角的x坐标
    for (vector<Vline>::iterator it = g_v_vline.begin() ; it != g_v_vline.end() ; it++)
    {
        // 寻找满足条件的x坐标最大的
        if (it->get_x() < rt_conner.x && it->pt_bottle.y < rt_conner.y
            && it->pt_top.y > as_bottle_y)
        {
            if (as_left_x < it->get_x() )
                as_left_x = it->get_x();
        }
    }
    action_space as(conner(as_left_x, as_bottle_y),rt_conner.x - as_left_x,
                    rt_conner.y - as_bottle_y );
    if (find(g_v_as.begin(), g_v_as_.end(), as) == g_v_as.end())
        g_v_as.push_back(as);

    // b1 先向左扩展，先寻找左边界，即左下角的x坐标
    as_bottle_y = MIN;
    as_left_x = MIN;
    for (vector<Vline>::iterator it = g_v_vline.begin() ; it != g_v_vline.end() ; it++)
    {
        // 寻找满足条件的x坐标最大的
        if (it->get_x() < rt_conner.x && it->pt_top.y >= rt_conner.y
            && it->pt_bottle.y < rt_conner.y )
        {
            if (as_left_x < it->get_x() )
                as_left_x = it->get_x();
        }
    }

    // b2 寻找下边界，即左下角的y坐标
    for (vector<Hline>::iterator it = g_v_hline.begin() ;  it != g_v_hline.end(); ++it)
    {
        // 寻找满足条件的y坐标的最大的
        if (it->get_y() < rt_conner.y && it->pt_right.x > as_left_x
            && it->pt_left.x < rt_conner.x )
        {
            if(as_bottle_y < it->get_y() )
                as_bottle_y = it->get_y();
        }
    }
    action_space as2(conner(as_left_x, as_bottle_y),rt_conner.x - as_left_x,
                    rt_conner.y - as_bottle_y );
    if (find(g_v_as.begin(), g_v_as_.end(), as2) == g_v_as.end())
        g_v_as.push_back(as2);
}

// 右下角算法
void conner2as_rb(vector<rectangle>:: iterator i2chonse_rec,const conner & rb_conner)
{
    // a1 先向上扩展，寻找上界,即左上角的y坐标
    int as_top_y = MAX ;
    int as_left_x = MIN ;
    for (vector<Hline>::iterator it = g_v_hline.begin() ;  it != g_v_hline.end(); ++it)
    {
        // 寻找满足条件的y坐标的最小的
        if (it->get_y() > rb_conner.y && it->pt_right.x >= rb_conner.x
            && it->pt_left.x < rb_conner.x)
        {
            if(as_top_y > it->get_y() )
                as_top_y = it->get_y();
        }
    }

    // a2 寻找左边界，即左上角的x坐标
    for (vector<Vline>::iterator it = g_v_vline.begin() ; it != g_v_vline.end() ; it++)
    {
        // 寻找满足条件的x坐标最大的
        if (it->get_x() < rt_conner.x && it->pt_top.y > rb_conner.y
            && it->pt_bottle.y < as_top_y )
        {
            if(as_left_x < it->get_x() )
                as_left_x = it->get_x();
        }
    }
    action_space as( conner(as_left_x, rb_conner.y),rb_conner.x - as_left_x,
                     as_top_y - rb_conner.y);
    if (find(g_v_as.begin(), g_v_as_.end(), as) == g_v_as.end())
        g_v_as.push_back(as);

    // b1 先向左扩展，寻找左上角的x坐标
    as_top_y = MAX ;
    as_left_x = MIN ;
    for (vector<Vline>::iterator it = g_v_vline.begin() ; it != g_v_vline.end() ; it++)
    {
        // 寻找满足条件的x坐标最大的
        if (it->get_x() < rb_conner.x && it->pt_bottle.y <= rb_conner.y
            && it->pt_top.y > rb_conner.y)
        {
            if (as_left_x < it->get_x() )
                as_left_x = it->get_x();
        }
    }
    // b2 寻找上边界，即左上角的y坐标
    for (vector<Hline>::iterator it = g_v_hline.begin() ;  it != g_v_hline.end(); ++it)
    {
        // 寻找满足条件的y坐标最小的
        if (it->get_y() > rb_conner.y && it->pt_right.x > as_left_x
            && it->pt_left.x < rb_conner.x )
        {
            if (as_top_y > it->get_y() )
                as_top_y = it->get_y();
        }
    }
    action_space as2( conner(as_left_x, rb_conner.y),rb_conner.x - as_left_x,
                     as_top_y - rb_conner.y);
    if (find(g_v_as.begin(), g_v_as_.end(), as2) == g_v_as.end())
        g_v_as.push_back(as2);    
}



void find_conner_vline2downline(const Vline & vline, const Hline & down_line)
{
    if (vline.pt_top.y >= down_line.get_y() && vline.pt_bottle.y < down_line.get_y()
        && vline.get_x() >= down_line.pt_left.x && vline.get_x() <= down_line.pt_right.x)
    {
        // 如果垂直线是右沿线，那么和举行块下沿线组成角为左上角
        if (vline.line_type == RIGHT_LINE && vline.get_x()!= down_line.pt_right.x)
            g_s_conner.insert(conner(vline.get_x(),down_line.get_y(),1,LEFT_TOP) );
        // 左沿线，右上角
        if(vline.line_type == LEFT_LINE && vline.get_x()!= down_line.pt_left.x ) 
            g_s_conner.insert(conner(vline.get_x(),down_line.get_y(),1,RIGHT_TOP) );
    }
}

void find_conner_vline2upline(const Vline & vline, const Hline & up_line)
{
    if (vline.pt_bottle.y <= up_line.get_y() && vline.pt_top.y > up_line.get_y()
        && vline.get_x() >= up_line.pt_left.x && vline.get_x() <= up_line.pt_right.x )
    {
        // 垂直线是左沿线且不和矩形上沿线的左端点重合，右下角
        if (vline.line_type == LEFT_LINE && vline.get_x() != up_line.pt_left.x)
            g_s_conner.insert(conner(vline.get_x(),up_line.get_y(),1,RIGHT_BOTTLE) );
        // 垂直线是右沿线且不和矩形上沿线的右端点重合，左下角
        if (vline.line_type == RIGHT_LINE && vline.get_x() != up_line.pt_right.x)
            g_s_conner.insert(conner(vline.get_x(),up_line.get_y(),1,LEFT_BOTTLE) );
    }
}

void find_conner_hline2leftline(const Hline & hline, const Vline & left_line)
{
    if (hline.pt_right.x >= left_line.get_x() && hline.pt_left.x < left_line.get_x()
        && hline.get_y() >= left_line.pt_bottle.y && hline.get_y() <= left_line.pt_top.y)
    {
        // 如果水平线是上沿线，则和矩形块的左沿线组成角为右下角
        if (hline.line_type == UP_LINE && hline.get_y() != left_line.pt_top.y)
            g_s_conner.insert(conner(left_line.get_x(),hline.get_y(),1,RIGHT_BOTTLE));
        // 如果水平线是下沿线，则和矩形块的左沿线组成角为右上角
        if (hline.line_type == DOWN_LINE && hline.get_y() != left_line.pt_bottle.y )
            g_s_conner.insert(conner(left_line.get_x(),hline.get_y(),1,RIGHT_TOP));
    }
}

void find_conner_hline2rightline(const Hline & hline, const Vline & right_line)
{
    if (hline.pt_left.x <= right_line.get_x() && hline.pt_right.x > right_line.get_x()
        && hline.get_y() >= right_line.pt_bottle.y && hline.get_y() <= right_line.pt_top.y)
    {
        // 水平线是上沿线，且不和当前矩形右沿线的上端点重合，左下角
        if (hline.line_type == UP_LINE && hline.get_y() != right_line.pt_top.y )
            g_s_conner.insert(conner(right_line.get_x(), hline.get_y(),1,LEFT_BOTTLE ));
        // 水平线是下沿线，且不和当前矩形右沿线的下端点重合，左上角
        if (hline.line_type == DOWN_LINE && hline.get_y() != right_line.pt_bottle.y)
            g_s_conner.insert(conner(right_line.get_x(), hline.get_y(),1,LEFT_TOP ));
    }
}


// 放置动作空间的左下角，可能产生的新的角
void generate_conners_lb(vector<rectangle>::iterator i2chonse_rec,
                         vector<action_space>::iterator i2chonse_as,
                         const conner & lb_conner    )
{
    // 这4条线
    g_v_hline.push_back(Hline(i2chonse_rec->left_top(),i2chonse_rec->right_top(),UP_LINE));
    g_v_hline.push_back(Hline(i2chonse_rec->left_bottle,i2chonse_rec->right_bottle(),DOWN_LINE));
    g_v_vline.push_back(Vline(i2chonse_rec->left_bottle, i2chonse_rec->left_top() ,LEFT_LINE) );
    g_v_vline.push_back(Vline(i2chonse_rec->right_bottle(), i2chonse_rec->right_top() ,RIGHT_LINE) );

    // 矩形块的左上角
    conner lt_conner(lb_conner.x,lb_conner.y + it->height,-1,-1 );
    Hline upl(i2chonse_rec->left_top(),i2chonse_rec->right_top(),UP_LINE);
    Hline downl(i2chonse_rec->left_bottle,i2chonse_rec->right_bottle(),DOWN_LINE);
    Vline leftl(i2chonse_rec->left_bottle, i2chonse_rec->left_top() ,LEFT_LINE);
    Vline rightl(i2chonse_rec->right_bottle(), i2chonse_rec->right_top() ,RIGHT_LINE);
    int mark = 0 ;
    for (vector<Vline>::iterator it = g_v_vline.begin(); it!= g_v_vline.end(); ++it)
    {
        // 如果动作空间的高度比小矩形大
        if (i2chonse_rec->height < i2chonse_as->height ) 
        {
            // 与动作空间左边形成的夹角
            if (it->get_x() == lt_conner.x && it->pt_bottle.y <= lt_conner.y
            && it->pt_top.y > lt_conner.y)
            {
                g_s_conner.insert(conner(lb_conner.x,lt_conner.y,1,LEFT_BOTTLE) );
            }
        }
        else // 动作空间高度和小矩形一样
        {
            // 在这种情况下，就要考虑垂直线和小矩形上沿所构成的角
            if (it->pt_bottle.y == upl.get_y() && it->get_x() >= upl.pt_left.x
                && it->get_x() <= upl.pt_right.x)
            {
                // 垂直线是左沿线且不和矩形上沿线的左端点重合，右下角
                if (it->line_type == LEFT_LINE && it->get_x() != upl.pt_left.x)
                    g_s_conner.insert(conner(it->get_x(),upl.get_y(),1,RIGHT_BOTTLE) );
                // 垂直线是右沿线且不和矩形上沿线的右端点重合，左下角
                if (it->line_type == RIGHT_LINE && it->get_x() != upl.pt_right.x)
                    g_s_conner.insert(conner(it->get_x(),upl.get_y(),1,LEFT_BOTTLE) );
            }
            // 以及矩形块的右上角是否能和动作空间构成一个左上角
            if (mark == 0 )
            {
                mark = 1;
                for (vector<Hline>::iterator itl = g_v_hline.begin(); itl != < g_v_hline.end(); ++itl)
                {
                    if ( itl->pt_left.x <= rightl.get_x() && itl->pt_right.x > rightl.get_x()
                        && itl->get_y() == rightl.pt_top.y )
                    {
                        g_s_conner.insert(conner(it->get_y(),rightl.get_x(),1,LEFT_TOP) );
                        break;
                    }
            
                }
            }
        }

        // 矩形块下沿线与其它垂直线形成的夹角
        if (it->pt_top.y == downl.get_y() && it->get_x() >= downl.pt_left.x
            && it->get_x() <= downl.pt_right.x)
        {
            // 如果垂直线是右沿线，那么和举行块下沿线组成角为左上角
            if (it->line_type == RIGHT_LINE && it->get_x()!= downl.pt_right.x)
                g_s_conner.insert(conner(it->get_x(),downl.get_y(),1,LEFT_TOP) );
            // 左沿线，右上角
            if(it->line_type == LEFT_LINE && it->get_x()!= downl.pt_left.x ) 
                g_s_conner.insert(conner(it->get_x(),downl.get_y(),1,RIGHT_TOP) );
        }
    } // end for

    mark = 0 ;
    int cn_rb_mark = 0 ;
    for (vector<Hline>::iterator it = g_v_hline.begin(); it != g_v_hline.end() ; ++it)
    {
        // 与动作空间下边形成的夹角,这个角只要条件满足，就必然存在
        if (cn_rb_mark == 0)
        {
            if (it->get_y() == rightl.pt_bottle.y && it->pt_left.x <= rightl.get_x()
                && it->pt_right.x > rightl.get_x())
            {
                g_s_conner.insert(conner(rightl.get_x(),it->get_y(),1,LEFT_BOTTLE));
                cn_rb_mark == 1 ;
            }
        }
         // 动作空间宽度和小矩形一样
        if (i2chonse_rec->width == i2chonse_as->height )
        {
            // 水平线和小矩形的右沿线所构成的角
            if (it->pt_left.x == rightl.x && it->get_y() >= rightl.pt_bottle.y
                && it->get_y() <= rightl.pt_top.y)
            {
                // 水平线是上沿线，且不和当前矩形右沿线的上端点重合，左下角
                if (it->line_type == UP_LINE && it->get_y() != rightl.pt_top.y )
                    g_s_conner.insert(conner(rightl.get_x(), it->get_y(),1,LEFT_BOTTLE ));
                // 水平线是下沿线，且不和当前矩形右沿线的下端点重合，左上角
                if (it->line_type == DOWN_LINE && it->get_y() != rightl.pt_bottle.y)
                    g_s_conner.insert(conner(rightl.get_x(), it->get_y(),1,LEFT_TOP ));
            }
            // 矩形块的右上交是否能和动作空间构成一个右下角
            if (mark == 0)
            {
                mark = 1;
                for (vector<Vline>::iterator itv = g_v_vline.begin(); itv !=g_v_vline.end() ; ++itv)
                {
                    if (itv->get_x() == upl.pt_right.x && itv->pt_bottle.y <= upl.get_y()
                        && itv->pt_top.y > upl.get_y())
                    {
                        g_s_conner.insert(conner(upl.pt_right.x,it->get_y(),1,RIGHT_BOTTLE ));
                        break;
                    }
                }
            }
        }

        // 矩形块左沿线与其它水平线形成的夹角
        if (it->pt_right.x == leftl.get_x() && it->get_y() >= leftl.pt_bottle.y
            && it->get_y() <= leftl.pt_top.y)
        {
            // 如果水平线是上沿线，则和矩形块的左沿线组成角为右下角
            if (it->line_type == UP_LINE && it->get_y() != leftl.pt_top.y)
                g_s_conner.insert(conner(leftl.get_x(),it->get_y(),1,RIGHT_BOTTLE));
            // 如果水平线是下沿线，则和矩形块的左沿线组成角为右上角
            if (it->line_type == DOWN_LINE && it->get_y() != leftl.pt_bottle.y )
                g_s_conner.insert(conner(leftl.get_x(),it->get_y(),1,RIGHT_TOP));
        }
        
    }
}

// 放置动作空间的左上角，可能产生的新的角
void generate_conners_lt(vector<rectangle>::iterator i2chonse_rec,
                         vector<action_space>::iterator i2chonse_as,
                         const conner & lt_conner    )
{
       // 矩形块的左上角
    conner lt_conner(lb_conner.x,lb_conner.y + it->height,-1,-1 );
    Hline upl(i2chonse_rec->left_top(),i2chonse_rec->right_top(),UP_LINE);
    Hline downl(i2chonse_rec->left_bottle,i2chonse_rec->right_bottle(),DOWN_LINE);
    Vline leftl(i2chonse_rec->left_bottle, i2chonse_rec->left_top() ,LEFT_LINE);
    Vline rightl(i2chonse_rec->right_bottle(), i2chonse_rec->right_top() ,RIGHT_LINE);
    int mark = 0 ;

    for (vector<Vline>::iterator it = g_v_vline.begin(); it != g_v_vline.end(); ++it)
    {
        // 如果动作空间的高度比小矩形大，才可能存在这个角
        if (i2chonse_rec->height < i2chonse_as->height ) 
        {
            // 与动作空间左边形成的夹角,左上角
            if (it->get_x() == leftl.get_x() && it->pt_bottle.y < leftl.pt_bottle.y
                && it->pt_top.y >= leftl.pt_bottle.y)
                g_s_conner.insert(conner(leftl.get_x(), leftl.pt_bottle.y,1,LEFT_TOP))
        }
        else // 动作空间高度和小矩形一样
        {
            // 在这种情况下，就要考虑垂直线和小矩形下沿所构成的角
            find_conner_vline2downline(*it,downl);
            // 以及矩形块的右下角是否能和动作空间构成一个右下角
            if (mark == 0)
            {
                mark = 1;
                for (vector<Hline>::iterator ith = g_v_hline.begin(); ith!= g_v_hline.end(); ++ith)
                {
                    if (ith->get_y() == rightl.pt_bottle.y && ith->pt_left.x <= rightl.get_x()
                        && ith->pt_right.x > rightl.get_x())
                    {
                        g_s_conner.insert(conner(rightl.get_x(),rightl.pt_bottle.y,1,LEFT_BOTTLE));
                        break;
                    }
                }
            }
        }
        
        // 矩形块上沿与其它垂直线形成的夹角
        find_conner_vline2upline(*it,upl);
    } // end for
    
    mark = 0 ;
    int cn_lt_mark = 0 ;
    for (vector<HLine>::iterator it = g_v_hline.begin(); it != g_v_hline.end(); it++)
    {
        // 与动作空间上边所形成的夹角,这个角只要条件满足就存在的
        if (cn_lt_mark == 0 )
        {
            if (it->get_y() == rightl.pt_top.y && it->pt_left.x <=rightl.get_x()
                && it->pt_right.x > rightl.get_x())
            {
                g_s_conner.insert(conner(rightl.get_x(),rightl.pt_top.y,1,LEFT_TOP));
                cn_lt_mark = 1;
            }
        }
        
        
    }
    
        
}


void generate_conners(vector<rectangle>::iterator i2chonse_rec,
                         vector<action_space>::iterator i2chonse_as)
{
    Hline upl(i2chonse_rec->left_top(),i2chonse_rec->right_top(),UP_LINE);
    Hline downl(i2chonse_rec->left_bottle,i2chonse_rec->right_bottle(),DOWN_LINE);
    Vline leftl(i2chonse_rec->left_bottle, i2chonse_rec->left_top() ,LEFT_LINE);
    Vline rightl(i2chonse_rec->right_bottle(), i2chonse_rec->right_top() ,RIGHT_LINE);

    for (vector<Vline>::iterator it = g_v_vline.begin(); it != g_v_vline.end(); ++it)
    {
        find_conner_vline2upline(*it,upl);
        find_conner_vline2downline(*it,downl);
    }

    for (vector<Hline>::iretator it = g_v_hline.begin(); it != g_v_hline.end(); ++it)
    {
        find_conner_hline2leftline(*it,leftl);
        find_conner_hline2rightline(*it,rightl);
    }
    
}


void update_action_space(vector<rectangle>::iterator i2chonse_rec,
                         vector<action_space>::iterator i2chonse_as )
{
    
}

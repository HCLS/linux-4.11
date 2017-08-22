#ifndef _LINUX_FS_STRUCT_H
#define _LINUX_FS_STRUCT_H

#include <linux/path.h>
#include <linux/spinlock.h>
#include <linux/seqlock.h>

// 프로세스의 task strut 마다 가르키고 있는 구조체
// fork() 시 CLONE_FS 플래그를 주면 이 구조체를
// 공유하는 프로세스가 생성된다. users 필드는 
// 이 구조체를 공유하는 프로세스의 수 이며, root
// 디렉토리 path 와 현재 프로세스의 working 디렉토리
// path를 root, pwd 에 저장한다.
struct fs_struct {
	int users;
	spinlock_t lock;
	seqcount_t seq;
	int umask;
	int in_exec;
	struct path root, pwd;
};

extern struct kmem_cache *fs_cachep;

extern void exit_fs(struct task_struct *);
extern void set_fs_root(struct fs_struct *, const struct path *);
extern void set_fs_pwd(struct fs_struct *, const struct path *);
extern struct fs_struct *copy_fs_struct(struct fs_struct *);
extern void free_fs_struct(struct fs_struct *);
extern int unshare_fs_struct(void);

// fs->root를 root에 복사하고, 레퍼런스 카운트 증가
static inline void get_fs_root(struct fs_struct *fs, struct path *root)
{
	spin_lock(&fs->lock);
	*root = fs->root;
	path_get(root);
	spin_unlock(&fs->lock);
}

static inline void get_fs_pwd(struct fs_struct *fs, struct path *pwd)
{
	spin_lock(&fs->lock);
	*pwd = fs->pwd;
	path_get(pwd);
	spin_unlock(&fs->lock);
}

extern bool current_chrooted(void);

#endif /* _LINUX_FS_STRUCT_H */

/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2011-2017 Daniel De Matteis
 *  Copyright (C) 2023 Jesse Talavera-Greenberg
 *
 *  RetroArch is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with RetroArch.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include "alsathread.h"

bool alsa_thread_dead(alsa_thread_info_t *info)
{
   if (!info || !info->cond_lock)
      return false;

   return __atomic_load_n(&info->thread_dead, __ATOMIC_SEQ_CST);
}

void alsa_set_thread_dead(alsa_thread_info_t *info, bool dead)
{
   if (!info || !info->cond_lock)
      return;

   __atomic_store_n(&info->thread_dead, dead, __ATOMIC_SEQ_CST);
}

void alsa_thread_free_info_members(alsa_thread_info_t *info)
{
   if (info)
   {
      if (info->worker_thread)
      {
         slock_lock(info->cond_lock);
         alsa_set_thread_dead(info, true);
         slock_unlock(info->cond_lock);
         sthread_join(info->worker_thread);
      }
      if (info->buffer)
         fifo_free(info->buffer);
      if (info->cond)
         scond_free(info->cond);
      if (info->fifo_lock)
         slock_free(info->fifo_lock);
      if (info->cond_lock)
         slock_free(info->cond_lock);
      if (info->pcm)
         alsa_free_pcm(info->pcm);
   }
   /* Do NOT free() info itself; it's embedded within another struct
    * that will be freed. */
}

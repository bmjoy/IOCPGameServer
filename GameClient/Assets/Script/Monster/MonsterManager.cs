﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MonsterManager : MonoBehaviour
{
    public static MonsterManager instance;

    public Dictionary<int, Monster> Monsters = new Dictionary<int, Monster>();

    public GameObject Monster;

    private bool IsSet=false;
    private int _setId;
    private Vector3 _setPos;
    private Vector3 _setDir;

    private void Awake()
    {
        if(instance == null)
        {
            instance = this;
        }
        else
        {
            Destroy(this.gameObject);
        }
    }

    private void Update()
    {
        if(IsSet)
        {
            GameObject obj = Instantiate(this.Monster, _setPos, Quaternion.identity);
            obj.GetComponent<Monster>().SetId(_setId);
            IsSet = false;
        }
    }
    
    public void SetMonster(Monster _monster)
    {
        Monsters.Add(_monster.GetId(), _monster);
    }

    public void SearchMonster(int _id,Vector3 _pos,Vector3 _dir)
    {
        if (Monsters.ContainsKey(_id))
        {
            Monsters[_id].SetPos(_pos, _dir);
        }
        else
        {
            _setId = _id;
            _setPos = _pos;
            _setDir = _dir;
            IsSet = true;
        }
    }
}
